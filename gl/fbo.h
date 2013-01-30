#ifndef GL_FBO_H
#define GL_FBO_H

#include <gl/exception.h>
#include <gl/gl.h>

// TODO renderbuffers: http://www.songho.ca/opengl/gl_fbo.html

namespace gl {

  struct texture;
  struct rbo;
  

  // TODO size !
  class fbo  {
    GLuint id_;
    const GLsizei width_;
    const GLsizei height_;

    typedef fbo self;
  public:
    fbo(GLsizei w, GLsizei h);

    GLuint id() const;
    GLenum target() const;

    self& gen();
    self& del();

    GLsizei width() const;
    GLsizei height() const;

    const self& bind() const;
    const self& unbind() const;
    
    const self& enable() const;
    const self& disable() const;

    // stuff must be bound !
    const self& attach(GLenum attachmentPoint, const texture& t) const;
    const self& attach(GLenum attachmentPoint, const rbo& r) const;
    
    // convenience
    const self& attach_color(unsigned int i, const texture& t) const;
    const self& attach_color(unsigned int i, const rbo& r) const;

    const self& attach_depth(const texture& t) const;
    const self& attach_depth(const rbo& r) const;
    // TODO attach_stencil

    
    template<class Action>
    const self& render(const Action& action) const {
      bind().enable();
      action();
      disable().unbind();
      return *this;
    }
    

    struct exception : gl::exception { };
    
    const self& check() const throw(exception);
  };

}


#endif
