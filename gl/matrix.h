#ifndef GL_MATRIX_H
#define GL_MATRIX_H

#include <gl/gl.h>
#include <math/matrix.h>

namespace gl {


  namespace matrix {

    void mode(GLenum data);
    void push();
    void pop();
    
    void load(const math::mat44& );
    
    template<class Action>
    void safe(const Action& action) {
      push();
      action();
      pop();
    }
    
  }
}


#endif
