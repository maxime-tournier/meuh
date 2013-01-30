#include "matrix.h"

namespace gl {

  namespace matrix {
    void mode(GLenum data) {
      glMatrixMode( data );
    }
    
    void push() {
      glPushMatrix();
    }
    
    void pop() {
      glPopMatrix();
    }

    void load(const math::mat44& data) {
      glLoadMatrixd(data.data());
    }
    
  }

}
