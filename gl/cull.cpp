#include "cull.h"

#include <gl/enable.h>
#include <gl/get.h>

#include <cassert>

namespace gl {
  namespace cull {

    void enable() {
      gl::enable( GL_CULL_FACE );
    }

    void disable() {
      gl::disable( GL_CULL_FACE );
    }
    
    bool enabled() {
      return gl::get<bool>(GL_CULL_FACE );
    }
    

    void face(GLenum mode ) {
      glCullFace( mode );
    }
    
    GLenum face() {
      return gl::get<int>(GL_CULL_FACE_MODE);
    }
    
 
    void flip() {
      assert( enabled() );

      GLenum mode = face();
      switch( mode ) {
      case GL_FRONT: face(GL_BACK); break;
      case GL_BACK: face(GL_FRONT); break;
      default: assert( false ); break;
      }
    }
    
  }

}

