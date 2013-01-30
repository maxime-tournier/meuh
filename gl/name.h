#ifndef GL_NAME_H
#define GL_NAME_H

#include <gl/gl.h>

namespace gl {

  namespace name {

    void push(GLuint i);
    void pop();

    template<class Action>
    void with(GLuint i, const Action& action) {
      push(i);
      action();
      pop();
    }

    
  }
}


#endif
