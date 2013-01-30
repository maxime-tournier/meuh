#ifndef GL_TRANSLATE_H
#define GL_TRANSLATE_H

#include <math/types.h>

namespace gl {

  void translate(const math::vec3& data);
  void translate(math::real x, math::real y, math::real z);

}


#endif
