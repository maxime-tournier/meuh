#include "shade.h"

namespace gl {

  namespace shade {
    void model(GLenum mode) {
      glShadeModel(mode);
    }

    void smooth() { model(GL_SMOOTH); }
    void flat() { model(GL_FLAT); }
  }
  
}
