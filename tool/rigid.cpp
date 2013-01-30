#include "rigid.h"

#include <gl/frame.h>

#include <math/func.h>
#include <math/func/SE3.h>
#include <math/func/compose.h>

#include <core/noop.h>
#include <core/log.h>

namespace tool {
  
  math::SE3 rigid::frame() const { 
    // return coords;
    return math::func::rigid<>()( g() );
  }

  rigid::~rigid() { 
  }


  void rigid::frame(const math::SE3& gg) {
    // coords = g;
    std::get<0>(g()) = gg.rotation();
    std::get<1>(g()) = gg.translation();
  }

  void rigid::mesh(const geo::mesh& mesh) {
    geometry.mesh.reset( new geo::mesh(mesh) );
    visual.mesh.reset( new gl::mesh(mesh) );

    auto v = new coll::vertices;
    v->data = mesh.vertices;
    
    collision.primitive.reset( v );

    update = [&,v] {
      geometry.mesh->vertices.each([&](math::natural i) {
	  v->data(i) = frame() ( geometry.mesh->vertices( i ) );
	});
    };
  }
  
  void rigid::draw() const {
    gl::frame{ frame() }([&]{
	visual.mesh->draw();
      });
  }
  
  using namespace math;
  
  rigid::rigid() : update( core::noop() ) { }

  // rigid::momentum_type rigid::map(const math::vec3& local, const math::force& f) const {
  //   using namespace func;
  //   return dT( absolute<>(local) << func::rigid<>()) (g) (f);
  // }

  math::vec3 rigid::map::operator()(const config_type& g) const {
    return std::get<1>(g) + std::get<0>(g)(local);
  }
  

  rigid::map::push::push(const map& of, const config_type& at) 
    : at(at), local(of.local) { }
  
  math::vec3 rigid::map::push::operator()(const velocity_type& v) const {
    return std::get<1>(v) + std::get<0>(at)( std::get<0>(v).cross(local) );
  }
  

  rigid::map::pull::pull(const map& of, const config_type& at) 
    : at(at), local(of.local) {

  }
  
  rigid::momentum_type rigid::map::pull::operator()(const math::force& f) const {
    momentum_type res;

    std::get<0>(res) = local.cross( std::get<0>(at).inverse()(f.transpose()) ).transpose();
    std::get<1>(res) = f;
    
    return res;
  }
  
}
