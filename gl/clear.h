#ifndef GL_CLEAR_H
#define GL_CLEAR_H

#include <gl/gl.h>
#include <math/types.h>

namespace gl {

  void clear(GLbitfield mask);
  void clear_color(const math::vec3& );
  
  // TODO 4 values 
}


#endif
