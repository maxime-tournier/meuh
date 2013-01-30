#include "translate.h"

#include <gl/gl.h>
#include <math/vec.h>

namespace gl {
  
  void translate(const math::vec3& data) {
    glTranslated(data.x(), data.y(), data.z());
  }

  void translate(math::real x, math::real y, math::real z) {
    glTranslated(x, y, z);
  }




}
