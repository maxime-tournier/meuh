#include "clear.h"

#include <math/vec.h>

namespace gl {
  
  void clear(GLbitfield mask) { glClear( mask ); }

  void clear_color(const math::vec3& v) {
    glClearColor(v.x(), v.y(), v.z(), 0);
  }


}
