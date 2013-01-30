#include "vertex.h"

#include <gl/gl.h>
#include <math/vec.h>

namespace gl {
   void vertex(const math::vec3& data) {
    glVertex3dv(data.data());
  }

  void vertex(math::real x, math::real y, math::real z) {
    glVertex3d(x, y, z);
  }


}
