#include "color.h"

#include <gl/gl.h>
#include <math/vec.h>

#include <math/pi.h>

namespace gl {
  
  using namespace math;

  vec3 html(unsigned int x) {
    const unsigned int r = (0xff0000 & x) >> 16;
    const unsigned int g = (0x00ff00 & x) >> 8;
    const unsigned int b = (0x0000ff & x);
    
    return vec3( r/256.0, g/256.0, b/256.0 );
  }
  
  
  void color(const vec3& data) { glColor3dv(data.data()); }
  void color(const vec4& data) { glColor4dv(data.data()); }

  void color(real x, real y, real z) { glColor3d(x, y , z); }
  void color(real x, real y, real z, real alpha) { glColor4d(x, y , z, alpha); }

  vec3 white() { return vec3::Ones(); }
  vec3 black() { return vec3::Zero(); }

  vec3 grey() { return 0.5 * white(); }

  vec3 red() { return vec3(1, 0, 0); }
  vec3 green() { return vec3(0, 1, 0); }
  vec3 blue() { return vec3(0, 0, 1); }

  vec3 magenta() { return vec3(1, 0, 1); }
  vec3 yellow() { return vec3(1, 1, 0); }
  vec3 cyan(){ return vec3(0, 1, 1); }

  vec3 orange() { return html(0xffa331); }
  vec3 violet() { return vec3( 0.6, 0.1, 1); }


  static real ratio = 2.0 / 3.0;
  
  vec3 darker(const vec3& c) {
    return c - ratio * vec3::Ones();
  }
  
  vec3 lighter(const vec3& c) {
    return c + ratio * vec3::Ones();
  }
  

  vec4 transp(const vec3& c, math::real alpha) { 
    return vec4(c.x(), c.y(), c.z(), alpha);
  }
  
  vec3 hsv(const vec3& q) {
    // see http://en.wikipedia.org/wiki/HSL_and_HSV
    const real& h = std::fmod(q(0), 2*math::pi);		// hue
    const real& s = q(1);		// saturation
    const real& v = q(2);		// value
    
    real c = v * s;		// chroma
    
    real hh = h / (math::pi / 3);
    
    real x = c * ( 1 - std::abs( std::fmod(hh, 2) - 1) );
    
    vec3 tmp;
    
  
    if( hh < 0) tmp = vec3::Zero();
    else if(hh < 1 ) tmp = vec3(c, x, 0);
    else if(hh < 2 ) tmp = vec3(x, c, 0);
    else if(hh < 3 ) tmp = vec3(0, c, x);
    else if(hh < 4 ) tmp = vec3(0, x, c);
    else if(hh < 5 ) tmp = vec3(x, 0, c);
    else tmp = vec3(c, 0, x);
   
    
    real m = v - c;

    return vec3::Constant(m) + tmp;
  }

}
