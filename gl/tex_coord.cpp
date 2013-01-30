#include "tex_coord.h"

#include <gl/gl.h>
#include <math/vec.h>

namespace gl {
  using namespace math;
  
  void tex_coord(real u, real v) {
    glTexCoord2d(u, v);
  }

  // TODO others

}
