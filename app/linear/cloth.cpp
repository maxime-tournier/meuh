#include "cloth.h"

#include <math/vec.h>
#include <math/form.h>
#include <math/mat.h>

#include <gl/common.h>
#include <gl/color.h>
#include <gl/init.h>
#include <math/quat.h>

#include <phys/constant.h>

cloth::~cloth() { }

using namespace math;

void cloth::reset() {

  // place vertices
  for(math::natural i = 0 ; i < dofs.size(); ++i ) {
    const natural x = i / width;
    const natural y = i % width;
    
    dofs[i].g() = scale * vec3(y, 0, x) / width;
    dofs[i].v().setZero();
    dofs[i].p().setZero();
  }

  in_tree.clear();
}


math::real cloth::error2(const delta_type& delta) {
  using namespace math;
  real res = 0;
  
  core::each(boost::edges( mesh ), [&](mesh_type::edge_descriptor e) {
      natural s = boost::source(e, mesh);
      vec3 pos_s = dofs[ s ].g();
      if(!delta.empty()) pos_s += delta( s );

      natural t = boost::target(e, mesh);
      vec3 pos_t = dofs[ t ].g();
      if(!delta.empty()) pos_t += delta( t );
      
      vec3 diff = pos_t - pos_s;

      auto square = [&](real x) { return x * x; };
						
      res += diff.squaredNorm() - mesh[e].rest * mesh[e].rest;
    
    });

  return res;
}



cloth::cloth(math::natural w, math::natural h, math::real scale) 
  : 
  dofs(w * h),
  constraints( w * (h - 1) + h * ( w - 1), 
	       phys::constraint::bilateral(1) ),
  mesh( dofs.size() ),
  width(w),
  height(h),
  total_mass(1.0),
  scale(scale),
  color(0x0000ff)
{
  
  using namespace math;
  
  auto pos = [&](natural i) -> vec3& {
    return dofs[i].g();
  };

  auto vel = [&](natural i) -> vec3& {
    return dofs[i].v();
  };

  auto mom = [&](natural i) -> form3& {
    return dofs[i].p();
  };
  
  math::natural size = dofs.size();
  
  reset();

  // create edges
  natural e = 0;
  for(natural i = 0 ; i < size; ++i ) {
    const natural x = i / width;
    const natural y = i % width;
      
    bool ok_height = (x < height - 1);
    bool ok_width = (y < width - 1);
      
    if( ok_height ) {
      real rest = (pos(i) - pos(i + width)).norm();
	
      boost::add_edge(i, i + width, edge_type{rest, &constraints[e++]}, mesh);
    }

    if( ok_width ) { 
      real rest = (pos(i) - pos(i + 1)).norm();
      boost::add_edge(i, i + 1, edge_type{rest, &constraints[e++]}, mesh);
    }
    
  }

  assert( e == constraints.size() );
  

  geometry.clear();
  for(natural i = 0 ; i < size; ++i ) {
    const natural x = i / width;
    const natural y = i % width;

    bool ok_height = (x < height - 1);
    bool ok_width = (y < width - 1);

    if( ok_height && ok_width ) {
      geometry.quads.push_back(geo::mesh::quad{i, i + 1, i + width + 1, i + width } );
    }

  };
  
}


void cloth::draw() const {
  using namespace gl;
  using namespace math;

  line_width(3);
  gl::color( white() );
  disable(GL_LIGHTING);
  begin(GL_LINES, [&] {
      core::each( boost::edges(mesh), [&](mesh_type::edge_descriptor e) {
	  if( in_tree.find( mesh[e].key ) != in_tree.end() ) {
	    
	    vertex( dofs[ boost::source(e, mesh) ].g() );
	    vertex( dofs[ boost::target(e, mesh) ].g() );
	    
	  }
	});
    });
  enable(GL_LIGHTING );
  
  gl::color( html(color) );
  
  cull::enable();
  cull::face(GL_BACK);
  enable( GL_POLYGON_OFFSET_FILL );
  glPolygonOffset( 1.f, 1.f );
  begin(GL_TRIANGLES, [&] {
      core::each( geometry.quads, [&](const geo::mesh::quad& q) {
	  const vec3& a = dofs[ q.vertex[0] ].g();
	  const vec3& b = dofs[ q.vertex[1] ].g();
	  const vec3& c = dofs[ q.vertex[2] ].g();
	  const vec3& d = dofs[ q.vertex[3] ].g();
		
	  normal( (b - a).cross(c - a) );
	  vertex( a, b, c);
		
	  normal( -(b - a).cross(c - a) );
	  vertex( a, c, b);
		
	  normal( (c - a).cross(d - a) );
	  vertex( a, c, d);
		
	  normal( -(c - a).cross(d - a) );
	  vertex( a, d, c);
	});
    });
  disable( GL_POLYGON_OFFSET_FILL );
  cull::disable();

 

}

math::real cloth::particle_mass() const {
  assert( size() );
  return total_mass / size();
}


void cloth::mass(phys::system& system) {


  core::each(dofs, [&](const obj_type& obj) {
      system.mass(&obj, &obj).diagonal().setConstant( particle_mass() );
      system.resp(&obj, &obj).diagonal().setConstant( 1.0 / particle_mass() );
    });
  
}


math::natural cloth::size() const { return dofs.size(); }

phys::dof::force cloth::force() const {

  // TODO alloc
  phys::dof::momentum f( dofs.size() );
  
  // gravity
  core::each( dofs, [&](const obj_type& o) {
      f(&o) += - particle_mass() * phys::constant::g * math::vec3::UnitY();
    });

  
  return f;
}



void cloth::bilateral(phys::system& system) {
  using namespace math;
  core::each( boost::edges(mesh), [&](const mesh_type::edge_descriptor& e) {
      natural i = boost::source(e, mesh);
      natural j = boost::target(e, mesh);
	
      auto info = mesh[e];
	
      const vec3 diff = dofs[i].g() - dofs[j].g(); 
      
      if( diff.norm() < 1e-5 ) core::log("ALARRRM");
      
      const vec3 n = diff.normalized();
      
      auto& J = system.constraint.bilateral.matrix;

      real norm = 2 * std::sqrt( 2 * diff.squaredNorm() );
      
      real scale = 1/norm;
      
      J(info.key, &dofs[i]) = scale * 2 * diff.transpose(); //  n.transpose();
      J(info.key, &dofs[j]) = -2 * scale * diff.transpose(); // - n.transpose();
      
      auto& b = system.constraint.bilateral.values;
      b(info.key).setZero();
      
      auto& c = system.constraint.bilateral.corrections;
      c(info.key).setConstant( scale * (info.rest * info.rest - diff.squaredNorm())  );
      
    });
  
}
