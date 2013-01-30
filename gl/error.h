#ifndef GL_ERROR_H
#define GL_ERROR_H

#include <core/error.h>
#include <gl/gl.h>

namespace gl {
  
  
  struct error : core::error {
    error(const std::string& what);
    
    static GLenum get();
    static std::string string( GLenum );
    
    static void check();
  };


  // TOOD CPP ?
}




#endif
