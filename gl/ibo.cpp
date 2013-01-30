#include "ibo.h"

namespace gl {

  ibo::ibo(GLenum usage) : buffer_object(usage) { }

  void ibo::bind() {
    create();
    glBindBufferARB(target, handle());
  }

  void ibo::unbind() {
    glBindBufferARB(target, buffer_object::none );
  }


}
