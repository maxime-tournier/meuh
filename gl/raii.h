#ifndef GL_RAII_H
#define GL_RAII_H

#include <gl/gl.h>

namespace gl {
  namespace raii {
    
    // TODO add matrixmode
    struct matrix {
      matrix();
      ~matrix();
    };
    
    struct enable {
      const GLenum what;
      enable(GLenum );
      ~enable();
    };

    struct disable {
      const GLenum what;
      disable(GLenum );
      ~disable();
    };

    struct begin {
      begin(GLenum mode);
      ~begin();
    };
    
  }
}



#endif
