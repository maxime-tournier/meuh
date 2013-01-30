#include "name.h"

namespace gl {
  namespace name {

    void push(GLuint i){ glPushName(i); }
    void pop(){ glPopName(); }
      
  
  }
}
