#include "scale.h"

#include <gl/gl.h>
#include <math/vec.h>

namespace gl {

  void scale(math::real w) { scale(w, w, w); }
  
  void scale(math::real x, math::real y, math::real z ) {
    glScaled(x, y, z);
  }


  void scale(const math::vec3& data ) {
    scale(data.x(), data.y(), data.z());
  }



}
