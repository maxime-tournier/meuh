#include "texture.h"

#include <gl/common.h>

namespace gl {
  
  texture::texture(GLenum unit, GLenum target) 
    : id_(0), unit_(unit), target_(target)  { }
  
  texture& texture::gen() {
    if(!id()) { 
      glActiveTexture(unit());
      gl::enable(target());
      
      glGenTextures(1, &id_); 

      glBindTexture(target(), 0);
      gl::disable(target());
    }
    return *this;
  }
  

  texture& texture::gen(GLuint ext) {
    if(!id()) id_ = ext;
    return *this;
  }
  
  const texture& texture::unbind() const { 
    glActiveTexture(unit());
    gl::enable(target());
    glBindTexture(target(), 0);
    gl::disable(target());
    return *this;
  }

  const texture& texture::bind() const {
    glActiveTexture(unit());
    gl::enable(target());
    glBindTexture(target(), id());
    return *this;
  }
  

  GLuint texture::id() const { return id_; }
  GLenum texture::target() const { return target_; }
  GLenum texture::unit() const { return unit_; }
  unsigned int texture::unit_id() const { return unit() - GL_TEXTURE0; }
  
  texture& texture::send(const data& d ) {
    
    glTexImage2D(target(), d.level(), d.int_format(), 
		 d.width(), d.height(), d.border(), 
		 d.format(), d.type(), d.ptr());
    data_ = d;
    return *this;
  }
  
  const texture::data& texture::sent() const { 
    return data_;
  }

  texture::data::data() 
    : level_(0),
      int_format_(GL_RGBA8),
      width_(0),
      height_(0),
      border_(0),
      format_(GL_RGB),
      type_(GL_UNSIGNED_BYTE),
      ptr_(0) {

  }

   
  GLint texture::data::level() const {
    return level_;
  }

  texture::data& texture::data::level(GLint x) {
    level_ = x;
    return *this;
  }
  
  GLint texture::data::int_format() const {
    return int_format_;
  }

  texture::data& texture::data::int_format( GLint x) {
    int_format_ = x;
    return *this;
  }
            
  GLint texture::data::border() const {
    return border_;
  }

  texture::data& texture::data::border(GLint x){
    border_ = x;
    return *this;
  }
            
  GLenum texture::data::type() const {
    return type_;
  }

  texture::data& texture::data::type(GLenum x){
    type_ = x;
    return *this;
  }
      
  GLsizei texture::data::width() const {
    return width_;
  }

  texture::data& texture::data::width(GLsizei x) {
    width_ = x;
    return *this;
  }
            
  GLsizei texture::data::height() const {
    return height_;
  }

  texture::data& texture::data::height(GLsizei x){
    height_ = x;
    return *this;
  }
  
  GLenum texture::data::format() const {
    return format_;
  }

  texture::data& texture::data::format(GLenum x) {
    format_ = x;
    return *this;
  }
  
  const GLvoid* texture::data::ptr() const {
    return ptr_;
  }

  texture::data& texture::data::ptr(const GLvoid* x){
    ptr_ = x;
    return *this;
  }

  const texture& texture::param(GLenum name, GLint p) const {
    glTexParameteri(target(), name, p);
    return *this;
  }
  

  // const texture& texture::filter(GLint p) const {
  //   param(GL_TEXTURE_MIN_FILTER, p);
  //   param(GL_TEXTURE_MAG_FILTER, p);
  //   return *this;
  // }
  

  
  const texture& texture::nearest() const {
    param(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    param(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return *this;
  }


  const texture& texture::linear() const {
    param(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    param(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return *this;
  }

  
  const texture& texture::mimap() const {
    param(GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
    return *this;
  }



  const texture& texture::clamp() const {
    param(GL_TEXTURE_WRAP_S, GL_CLAMP);
    param(GL_TEXTURE_WRAP_T, GL_CLAMP);
    return *this;
  }

  const texture& texture::clamp_edge() const {
    param(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    param(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return *this;
  }


  
  texture::data texture::data::depth() {
    return data().int_format(GL_DEPTH_COMPONENT32).format(GL_DEPTH_COMPONENT);
  }


  const texture& texture::compare() const {
    return 
      param( GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE).
      param( GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL).
      param( GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
  }


}
