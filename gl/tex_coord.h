#ifndef GL_TEX_COORD_H
#define GL_TEX_COORD_H

#include <math/types.h>

namespace gl {

  void tex_coord(math::real x, math::real y);
  
  void tex_coord(math::real x, math::real y, math::real z);
  void tex_coord(const math::vec3& v);

}


#endif
