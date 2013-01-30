#include "fbo.h"

#include <gl/gl.h>

#include <gl/texture.h>
#include <gl/rbo.h>
#include <gl/error.h>


namespace gl {

  fbo& fbo::gen() {
    if( !id() ) glGenFramebuffersEXT(1, &id_);
    return *this;
  }
  
  fbo& fbo::del() {
    if( !id() ) glDeleteFramebuffersEXT(1, &id_);
    // id_ = 0; 			// assert ?
    return *this;
  }

  fbo::fbo(GLsizei w, GLsizei h) 
    :id_(0), width_(w), height_(h) { }
  
  GLuint fbo::id() const { return id_; }
  GLenum fbo::target() const { return GL_FRAMEBUFFER_EXT; }
  
  const fbo& fbo::bind() const {
    glBindFramebufferEXT(target(), id());
    return *this;
  }
  
  const fbo& fbo::unbind() const {
    glBindFramebufferEXT(target(), 0);
    return *this;
  }
  
  GLsizei fbo::width() const { return width_;}
  GLsizei fbo::height() const { return height_; }


  const fbo& fbo::attach(GLenum attachmentPoint, const texture& t) const {
    assert(t.sent().width() == width());
    assert(t.sent().height() == height());
    
    // TODO consistency checks (depth/texture...)
    glFramebufferTexture2DEXT(target(), attachmentPoint,
			      t.target(), t.id(), 0 );
    return *this;
  }

  const fbo& fbo::attach(GLenum attachmentPoint, const rbo& r) const {
     // TODO consistency checks (depth/texture...)
     assert(r.width() ==  width() );
     assert(r.height() == height() );

     glFramebufferRenderbufferEXT(target(), attachmentPoint,
				 r.target(), r.id() );
    return *this;
  }

  const fbo& fbo::attach_color(unsigned int i, const texture& t) const {
    return attach(GL_COLOR_ATTACHMENT0_EXT + i, t);
  }

  const fbo& fbo::attach_color(unsigned int i, const rbo& r) const {
    return attach(GL_COLOR_ATTACHMENT0_EXT + i, r);
  }
  
  
  const fbo& fbo::attach_depth(const texture& t) const {
    return attach(GL_DEPTH_ATTACHMENT_EXT, t);
  }

  const fbo& fbo::attach_depth(const rbo& r) const {
    return attach(GL_DEPTH_ATTACHMENT_EXT, r);
  }


  const fbo& fbo::enable() const {
    glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, width(), height());
    return *this;
  }

  const fbo& fbo::disable() const {
    glPopAttrib();
    return *this;
  }





#define macro_error( glerror  ) \
glerror: throw error( "fbo check: " #glerror ); \
  

//  GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT 0x8CD6
// #define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT 0x8CD7
// #define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT 0x8CD9
// #define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT 0x8CDA
// #define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT 0x8CDB
// #define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT 0x8CDC


  const fbo& fbo::check() const throw(exception) {
    GLenum res = glCheckFramebufferStatusEXT(target());

    switch( res ) {
    case GL_FRAMEBUFFER_COMPLETE_EXT: return *this;
    case macro_error( GL_FRAMEBUFFER_UNSUPPORTED_EXT );
    case macro_error( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT );
    case macro_error( GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT );
    case macro_error( GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT );
    case macro_error( GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT );
    case macro_error( GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT );
    case macro_error( GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT );
    
    default: 
      // std::cout << std::hex << res << std::endl;
      throw error("unknown");
    };
    
    return *this;
  }

}
