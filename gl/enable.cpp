#include "enable.h"


namespace gl {
  
  void enable(GLenum what) {
    glEnable( what );
  }

  void disable(GLenum what) {
    glDisable( what );
  }
  
  bool enabled(GLenum what) {
    return glIsEnabled(what);
  }

}
