#ifndef GL_UNIFORM_H
#define GL_UNIFORM_H

#include <math/types.h>
#include <gl/gl.h>

namespace gl {

  struct texture;

  struct uniform {
    const GLint location;
    
    uniform& operator=(const math::vec3& );
    uniform& operator=(int i);
    uniform& operator=(const texture& );
    
  };



}


#endif
