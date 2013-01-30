#include "rotate.h"

#include <gl/gl.h>

#include <math/pi.h>
#include <math/vec.h>
#include <math/so3.h>

namespace gl {
  
  
  void rotate(math::real angle, math::real x, math::real y, math::real z) {
    glRotated(angle / math::deg, x, y, z);
  }

  void rotate(const math::vec3& data) {
    rotate(data.norm(), data.x(), data.y(), data.z());
  }
  
  void rotate(const math::quat& data) {
    const math::quat flipped = data.flipped();
    
    const math::vec3& vec = flipped.vec();
    rotate(flipped.angle(), vec.x(), vec.y(), vec.z());
  }
  
  void rotate(const math::SO3& data) {
    rotate(data.quaternion());
  }



}
