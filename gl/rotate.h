#ifndef GL_ROTATE_H
#define GL_ROTATE_H

#include <math/types.h>

namespace gl {

  // NOTE: angle is in rad
  void rotate(math::real angle, math::real x, math::real y, math::real z);
  void rotate(const math::vec3& data);
  void rotate(const math::quat& data);
  void rotate(const math::SO3& data);
  
}


#endif
