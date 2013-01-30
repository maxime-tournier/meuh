#ifndef GL_SCALE_H
#define GL_SCALE_H

#include <math/types.h>

namespace gl {

  void scale(math::real w);
  void scale(math::real x, math::real y, math::real z );
  void scale(const math::vec3& data );

}


#endif
