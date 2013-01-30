#ifndef GL_COLOR_H
#define GL_COLOR_H

#include <math/types.h>
#include <math/vec.h>

namespace gl {

  void color(const math::vec3& data);
  void color(math::real x, math::real y, math::real z);  

  void color(const math::vec4& data);
  void color(math::real x, math::real y, math::real z, math::real alpha);  

  math::vec3 white();
  math::vec3 black();

  math::vec3 grey();

  math::vec3 red();
  math::vec3 green();
  math::vec3 blue();

  math::vec3 magenta();
  math::vec3 yellow();
  math::vec3 cyan();

  math::vec3 orange();
  math::vec3 violet();
  
  math::vec3 darker(const math::vec3& );
  math::vec3 lighter(const math::vec3& );
  
  math::vec3 html(unsigned int html);
  math::vec3 hsv(const math::vec3& );

  math::vec4 transp(const math::vec3& color, math::real alpha = 0.5);
  
}



#endif
