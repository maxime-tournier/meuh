#ifndef GL_VERTEX_H
#define GL_VERTEX_H

#include <math/types.h>

namespace gl {

  void vertex(const math::vec3& data);
  void vertex(math::real x, math::real y, math::real z = 0);

  template<class ... Tail>
  void vertex(const math::vec3& head, const Tail& ... tail) {
    vertex(head);
    vertex(tail...);
  }

}



#endif
