#ifndef GL_ENABLE_H
#define GL_ENABLE_H

#include <gl/gl.h>

namespace gl {

  void enable(GLenum what);
  void disable(GLenum what);
  
  
  bool enabled(GLenum what);

  template<class Action>
  void with(GLenum what, const Action& action) {
    // TODO pushattrib ?
    enable(what);
    action();
    disable(what);
  }


  template<class Action>
  void without(GLenum what, const Action& action) {
    // TODO pushattrib ?
    disable(what);
    action();
    enable(what);
  }

  
  
}



#endif
