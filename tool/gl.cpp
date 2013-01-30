#include "gl.h"

#include <gl/common.h>

namespace tool {
  
  gl::gl() 
    : api("gl") {
    using namespace math;
    using namespace ::gl;
    api
      .fun("clear_color", [&](real r, real g, real b) {
	  clear_color( vec3(r, g, b));
	})
      .fun("fog_color", [&](real r, real g, real b) {
	  fog::color( vec3(r, g, b));
	})
      .fun("fog_density", [&](real x){
	  fog::density( x );
	})
      .fun("ambient", [&](real r, real g, real b) {
	  light::model::ambient( vec3(r, g, b) );
	})
      ;
  }


}
