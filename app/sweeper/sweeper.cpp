
#include "sweeper.h"

#include <math/tuple.h>
#include <math/func/vector.h>
#include <math/func/tuple.h>


#include <gl/common.h>
#include <gl/init.h>
#include <math/random.h>

#include <math/func/jacobian.h>
#include <math/func/SE3.h>

#include <core/log.h>

#include <QKeyEvent>


#include <gl/frame.h>

#include <math/func/numerical.h>
#include <math/func/constant.h>

#include <tool/mesh.h>

#include <core/share.h>
#include <math/svd.h>

#include <core/macro/here.h>
#include <math/func/nan.h>
#include <core/stl.h>

#include <geo/mesh.h>
#include <geo/obj.h>

#include <math/func/lie/translation.h>
#include <gl/blend.h>

#include <tuple>
#include <utility>

#include <sparse/diag.h>

#include <gl/raii.h>

using namespace math;

namespace gui {


  gl::mesh sphere;
  geo::mesh bunny;
  math::vec3 bunny_center;
  
  // struct multi_map {
  //   typedef sweeper::sweeper_type::dof_type domain;
  //   typedef math::vector< math::vec3 > range; 

  //   const range& points;

  //   range operator()(const domain& g) const {
      
  //     range res; res.resize(points.rows());

  //     points.each([&](math::natural i) {
  // 	  res(i) = sweeper::sweeper_type::map{points(i)}(g);
  // 	});
      
  //     return res;
  //   }

  //   math::T<range> operator()(const math::T<domain>& dg) const {
      
  //     range at; at.resize(points.rows());
  //     range body; body.resize(points.rows());
      
  //     points.each([&](math::natural i) {
  // 	  auto dpi = sweeper::sweeper_type::map{points(i)}( dg );
  // 	  at(i) = dpi.at();
  // 	  body(i) = dpi.body();
  // 	});
      
  //     return math::body(std::move(at), std::move(body));
  //   }

  // };

  // using namespace math;

  // static const auto trans = math::func::tuple_get<0, sweeper::obj_type::coords_type>();
  // static const auto rot = math::func::tuple_get<1, sweeper::obj_type::coords_type>();
  
  // static const auto rigid = math::func::rigid<>() << math::func::join(rot, trans);
  // static const auto rigid = func::const_over<sweeper::obj_type::coords_type>(SE3::identity());
  
  // static const auto sweeper_dofs = math::func::tuple_get<2, sweeper::obj_type::coords_type>();

  // static const auto center = func::tuple_get<0, sweeper::sweeper_type::dof_type>() << sweeper_dofs;
  // static const auto axis = func::tuple_get<1, tool::sweeper::dof_type>() << sweeper_dofs;
  
  
  // struct potential {
  //   const math::vector< vec3 >& points;

  //   typedef sweeper::obj_type::coords_type domain;
  //   typedef real range;

  //   range operator()(const domain& g) const {
  //     using namespace math;

  //     range res = 0;

  //     points.each( [&](math::natural i) {
  // 	  auto diff = func::lie::L<vec3>( -points(i) ) << center;
  // 	  auto dist2 = func::euclid::norm2<vec3>() << diff;

  // 	  auto at_p = func::join( func::id<tool::sweeper::dof_type>(),
  // 				   func::const_over<tool::sweeper::dof_type>(points(i)) ) << sweeper_dofs;
	  
  // 	  auto twist = func::euclid::bi_scalar<vec3>() << func::join( tool::sweeper::weight() << at_p, axis);
  // 	  auto angle2 = func::euclid::norm2<vec3>() << twist;
	  
  // 	  auto full = func::euclid::bi_scalar<real>() 
  // 	    << func::join( dist2, angle2 );
	  
  // 	  res += full(g);
  // 	});
     
  //     return res;
  //   }
  // };


  // struct potential_euclid {
  //   const math::vector< vec3 >& points;

  //   typedef sweeper::obj_type::coords_type domain;
  //   typedef real range;

  //   range operator()(const domain& g) const {
  //     using namespace math;

  //     range res = 0;

  //     points.each( [&](math::natural i) {
  // 	  auto diff = (sweeper::sweeper_type::map{points(i)} << sweeper_dofs) - func::const_over<domain>(points(i));
  // 	  auto full = func::euclid::norm2<vec3>() << diff;

  // 	  res += full(g);
  // 	});
     
  //     return res;
  //   }
  // };


  // struct rigid_map { 
  //   typedef sweeper::obj_type::coords_type domain;
  //   typedef math::vec3 range;

  //   const math::vec3& p;
    
  //   range operator()(const domain& g) const {
      
  //     return (math::func::apply_rigid<>() << math::func::join(rigid, sweeper::sweeper_type::map{p}  << sweeper_dofs ))(g);
  //   }

  //   math::T<range> operator()(const math::T<domain>& dg) const {
      
  //     return (math::func::apply_rigid<>() << 
  //     	      math::func::join(rigid, sweeper::sweeper_type::map{p} << sweeper_dofs ))(dg);
  //   }

  // };



  // struct rigid_multi_map {
    
  //   typedef sweeper::obj_type::coords_type domain; 
  //   typedef math::vector< math::vec3 > range; 
    
  //   const range& points;
    

  //   range operator()(const domain& g) const {
  //     range res; res.resize(points.rows());
       
  //      points.each([&](math::natural i) {
  //      	  res(i) = rigid_map{ points(i) }(g);
       
  //      	});
       
  //     return res;
  //   }

  //   math::T<range> operator()(const math::T<domain>& dg) const {
  //     range at; at.resize(points.rows());
  //     range body; body.resize(points.rows());
    
  //     points.each([&](math::natural i) {
  //     	  auto dpi = rigid_map{ points(i) }(dg);
  //     	  at(i) = dpi.at();
  //     	  body(i) = dpi.body();
  //     	});
      
  //      return math::body(std::move(at), std::move(body));
  //   }


  // };


  // void sweeper::keyPressEvent(QKeyEvent* e) {
  //    switch(e->key()) {
  //    case Qt::Key_R: 
  //      reset = true;
  //      break;
  //    default:
  //      QGLViewer::keyPressEvent(e);
  //    }
  // }


  // struct kinetic {
    
  //   typedef std::tuple<sweeper::obj_type::coords_type,
  // 		       sweeper::obj_type::velocity_type> domain;
  //   typedef math::real range;

  //   const math::vector<math::vec3>& points;
    
  //   range operator()(const domain& x) const {
  //     const auto& g = std::get<0>(x);
  //     const auto& v = std::get<1>(x);
      
  //     return 0.5 * math::euclid::norm2( rigid_multi_map{points}( math::body(g, v) ).body() );
  //   }
    
  // };



  // void sweeper::animate() { 
    
    // using namespace math;
    
    // if( reset ) {
    //   obj->reset();
    //   reset = false;
    // }

    // auto maps = rigid_multi_map{points};

    // auto G = lie::of(obj->coords);

    // if( nan(obj->coords) ) throw std::logic_error(macro_here);

    // real dt = 0.02;

    // mat Jtot = func::J( maps, obj->lie, lie::of(points) )( obj->coords );
    // if( nan(Jtot) ) throw std::logic_error(macro_here);
    
    // mat Morig = Jtot.transpose() * Jtot;

    // real stiffness = 1;
    // real damping = 1;

    // mat M = (1 + damping*dt ) * Morig;
    

    // // we apply a test force on first point along y
    // vec f = vec::Zero(obj->lie.alg().dim());

    // for(natural i = 0; i < frames.size(); ++i) {
    //   vec3 p = rigid_map{points(i)}( obj->coords );
    //   f += 2e4 * Jtot.block(3*i, 0, 3, Jtot.cols()).transpose() * (frames[i]->transform().translation() - p);
    // }
    
   

    // vec grad = func::J(func::num( potential{points} ), obj->lie)(obj->coords).transpose();
    // f -= stiffness * grad;

  
    
    // vec at = obj->lie.alg().coords( lie::log( obj->coords ));
    // core::log()("at:", at.tail( obj->lie.alg().dim() - 6 ).transpose());
    
   
    
    // if( nan(f) ) throw std::logic_error(macro_here);
    
    // typedef obj_type::coords_type Q;
    // auto partial = func::join( func::id<Q>(), func::const_over<Q>(obj->velocity)   );

    // vec inertia = func::J( func::num( kinetic{points} << partial), obj->lie )(obj->coords).transpose();
    // f += inertia;
    
    // // core::log()("inertia:", inertia.norm());
    // // core::log()("f:", f.tail(7).transpose() );
    // if( nan(f) ) throw std::logic_error(macro_here);
    
    // mat Mi = math::svd(M, 1e-2).inverse();
    // vec v = Mi * G.alg().coords(obj->impulse(G.alg().elem(f), dt));
    // if( nan(v) ) throw std::logic_error(macro_here);

    // vec mu = Morig * v;
    
    // obj->integrate(G.alg().elem(v), G.alg().elem(mu), dt);
    
    // auto dp = maps( obj->state() );

    // pos = dp.at();
    // vel = dp.body();
  // }
 
 

  static void draw(const tool::sweeper::dof_type& dofs) {
    using namespace gl;
    
    begin(GL_POINTS, [&] {
	color(blue());
	vertex(std::get<0>(dofs));
      });
    
    begin(GL_LINES, [&] {
	color(yellow());
	vertex( std::get<0>(dofs), std::get<0>(dofs) + std::get<1>(dofs) );
      });
    
  }


  // static void draw(const tool::symmetric::dof_type& ) {

  // }



   void sweeper::draw() { 
    using namespace gl;
    
    if( pos.empty() ) return;
    
    enable(GL_LIGHTING );
    
    
    {
      raii::matrix safe;
      translate( std::get<0>(obj->conf) );
      QGLViewer::drawAxis();
      
      begin(GL_LINES, [&]{ 
	  vertex(vec3::Zero());
	  vertex( std::get<1>(obj->conf ) );
	});
    }

    core::each(frames, [&](gui::frame* f) {
	f->draw(10);
      });
    
    color( transp( white()));
    blend()( [&] {
	begin(GL_TRIANGLES, [&] {
	    
	    for( math::natural t = 0; t < bunny.triangles.size(); ++t) {
	      auto& vert = bunny.triangles[t].vertex;
	      
	      normal( (pos( vert[1] ) - pos( vert[0] )).cross(pos( vert[2] ) - pos( vert[0] ) ) );
	      
	      vertex( pos( vert[0] ),
	      	      pos( vert[1] ),
	      	      pos( vert[2] ) );
	      
	    };
	  });
      });

    
    color( white() );
    point_size( 4 );
	
    // pos.each([&](math::natural i) {
    // 	matrix::push();
    // 	translate( pos(i) );
	  
    // 	// velocities
    // 	// disable(GL_LIGHTING );
    // 	// begin(GL_LINES, [&] {
    // 	//     vertex( math::vec3::Zero(), vel(i) );
    // 	//   });
    // 	// enable(GL_LIGHTING );
	    
    // 	scale(0.1);
    // 	sphere.draw();
    // 	matrix::pop();
    //   });

    disable(GL_LIGHTING );
    point_size(8);

    // draw mapped points
    begin(GL_POINTS, [&] {
	color(green());
	for(math::natural i = 0; i < frames.size(); ++i) { vertex( pos(i) ); }
      });

    // draw dofs
    // gl::frame{ rigid(obj->conf) }( [&] {
    // 	gui::draw( dofs );
    //   });
       
  
    

   

   }

  phys::dof::momentum sweeper::momentum(math::real t, math::real dt) {
    
    phys::dof::momentum res = engine.momentum(t, dt);

    for(natural i = 0; i < frames.size(); ++i) {

      vec3 diff = frames[i]->transform().translation() - pos(i);
      real stiffness = 100;

      auto map = tool::sweeper() << func::join( func::id< tool::sweeper::dof_type>(), 
						func::const_over<tool::sweeper::dof_type>( points(i) ) );

      // TODO don't build full J lol
      mat J = func::J(map)( obj->conf );
      res( obj.get() ) += dt * (J.transpose() * diff);
      
    }

    return res;
  }


  phys::dof::velocity sweeper::solve(const phys::dof::momentum& p) {
    return sparse::diag( system.resp ) * p;
  }
  
  void sweeper::integrate(const phys::dof::velocity& v, math::real dt) {
    
    engine.integrate(v, sparse::diag( system.mass ) * v, dt );
    
    points.each([&](natural i) {
	auto map = tool::sweeper() << func::join( func::id< tool::sweeper::dof_type>(), 
						  func::const_over<tool::sweeper::dof_type>( points(i) ) );
	pos(i) = map( obj->conf );
      });
    
  }


  void sweeper::setup() {
    
    // mass setup
    auto M = system.mass( obj.get(), obj.get() );

    points.each([&](natural i) {
	auto map = tool::sweeper() << func::join( func::id< tool::sweeper::dof_type>(), 
						  func::const_over<tool::sweeper::dof_type>( points(i) ) );
	mat J = func::J(map)( obj->conf );

	M.noalias() += J.transpose() * J;
      });

    M += mat66::Identity();
    
    system.resp( obj.get(), obj.get() ) = M.inverse();

  };

  void sweeper::reset() {
    engine = phys::engine();
    system = phys::system();
    
    obj.reset( new obj_type  );
    
    engine.add( obj.get() );

    pos = points;
    vel = vector<vec3>::Zero( pos.rows() );

    post_steps = 0;
  }

  void sweeper::init() {
    using namespace math;
    
    
    sphere = tool::load_obj( core::share("/mesh/box.obj" ) );
    bunny = geo::obj::load(core::share("/mesh/bunny.obj" ));

    bunny_center.setZero();
    bunny.vertices.each([&](math::natural i) {
	bunny_center += bunny.vertices(i);
      });

    bunny_center /= bunny.vertices.rows();
    bunny.vertices.each([&](math::natural i) {
	bunny.vertices(i) -= bunny_center;
	bunny.vertices(i) *= 5;
      });
    
    points = bunny.vertices;

    const natural n = 4;
    for(natural i = 0; i < n; ++i) {
      assert( !points.empty());
      frame* f = new gui::frame;
      f->transform( math::SE3::translation( points(i) ) );
      frames.push_back( f );
    }

    setSceneRadius( 1000 );
    setMouseTracking(true);

    reset();
    startAnimation();

  }

}
