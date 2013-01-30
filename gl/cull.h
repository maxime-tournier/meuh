#ifndef GL_CULL_H
#define GL_CULL_H

#include <gl/gl.h>

namespace gl {
  namespace cull {

    void enable();
    void disable();
    bool enabled();
    

    void face(GLenum);
    GLenum face();
    
    void flip();
    
  }

}


#endif
