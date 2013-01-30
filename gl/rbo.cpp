#include "rbo.h"

namespace gl {
  
  rbo::rbo() : id_(0), width_(0), height_(0), format_(0) {}


  GLuint rbo::id() const { return id_; }
  GLenum rbo::target() const { return GL_RENDERBUFFER_EXT; }

  rbo::self& rbo::gen() {
    glGenRenderbuffersEXT(1, &id_);
    return *this;
  }
  rbo::self& rbo::del() {
    glDeleteRenderbuffersEXT(1, &id_);
    return *this;
  }
  
  const rbo::self& rbo::bind() const {
    glBindRenderbufferEXT(target(), id() );
    return *this;
  }
  
  const rbo::self& rbo::unbind() const {
    glBindRenderbufferEXT(target(), 0);
    return *this;
  }

  GLsizei rbo::width() const { return width_; }
  GLsizei rbo::height() const { return height_; }
  GLenum rbo::format() const { return format_; }
   
  rbo::self& rbo::storage(GLenum format, GLsizei width, GLsizei height) {
    width_ = width;
    height_ = height;
    format_ = format;
    
    glRenderbufferStorageEXT(target(), format, width, height);
    return *this;
  }
 
  
  

}
