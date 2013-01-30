#ifndef GL_BEGIN_H
#define GL_BEGIN_H

#include <gl/gl.h>

namespace gl {

  template<class Action>
  void begin(GLenum mode, const Action& action) {
    glBegin( mode );
    action();
    glEnd();
  }

}


#endif
