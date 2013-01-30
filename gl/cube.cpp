#include "cube.h"

#include <gl/vbo.h>

#include <math/vec.h>
#include <math/so3.h>
#include <math/covector.h>

#include <math/pi.h>

namespace gl {

  std::unique_ptr<vbo::data_type> cube() { 
    
    const math::real u = 0.5;
    
    typedef array<math::vec3, math::vec3> vbo_type;
    vbo_type* res = new vbo_type(24);
    res->layout().vertex<0>().normal<1>();
    
    auto vertex = res->access<0>();
    auto normal = res->access<1>();

    using namespace math;
    
    int v_off = 0;
    int n_off = 0;

    math::lie::group<SO3> lie;

    for(natural coord = 0; coord < 3; ++coord) {
      for(real c = -1; c <= 1; c += 2 ) {
	
	vec3 n = vec3::Zero();
	n(coord) = c;

	SO3 g = lie.exp()( half_pi * n );
	
	vec3 v(u, u, u);
	if( v.dot(n) < 0 ) v = -v;
	
	for( int i = 0; i < 4; ++i) {
	  vertex( v_off++ ) = v; 
	  v = g(v);
	  
	  normal( n_off++ ) = n;
	}
	
      }
    }
    
    return std::unique_ptr<vbo::data_type>(res);
  }

}
