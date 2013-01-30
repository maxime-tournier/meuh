#ifndef GL_RBO_H
#define GL_RBO_H

#include <gl/gl.h>

namespace gl {

  class rbo  {
    GLuint id_;
    typedef rbo self;

    GLsizei width_;
    GLsizei height_;
    GLenum format_;
  public:
    rbo();

    GLuint id() const;
    GLenum target() const;

    self& gen();
    self& del();

    const self& bind() const;
    const self& unbind() const;

    GLsizei width() const;
    GLsizei height() const;
    GLenum format() const;
    
    // internal format
    self& storage(GLenum format, GLsizei width, GLsizei height);

  };


}


#endif
