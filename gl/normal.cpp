#include "normal.h"

#include <gl/gl.h>
#include <math/vec.h>

namespace gl {
  using namespace math;

  void normal(real x, real y, real z) {
    glNormal3d(x, y, z);
  }


  void normal(const vec3& v) {
    glNormal3dv(v.data());
  }
  

}
