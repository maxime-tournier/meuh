#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#include <gl/gl.h>
#include <gl/enable.h>

namespace gl {

  class texture {
    GLuint id_;
    GLenum unit_;
    GLenum target_;

    typedef texture self;
    
  public:
    
    texture(GLenum unit = GL_TEXTURE0, GLenum target = GL_TEXTURE_2D);
    
    GLuint id() const;
    GLenum target() const;
    GLenum unit() const;
    unsigned int unit_id() const;
    
    self& gen();
    self& gen(GLuint i);
    self& del();		// TODO

    const self& bind() const;
    const self& unbind() const;
    
    // bind first, then...
    const self& param(GLenum name, GLint parm) const;

    // convenience:
    const self& nearest() const;
    const self& linear() const;
    const self& mimap() const;

    const self& clamp() const;
    const self& clamp_edge() const;
    

    class data {
      GLint  	level_;
      GLint  	int_format_;
      GLsizei  	width_;
      GLsizei  	height_;
      GLint  	border_;
      GLenum  	format_;
      GLenum  	type_;
      const GLvoid*   ptr_;
    public:
      GLint level() const;
      data& level(GLint );
      
      GLint int_format() const;
      data& int_format( GLint );
            
      GLint border() const;
      data& border(GLint );
            
      GLenum type() const;
      data& type(GLenum );
      
      GLsizei width() const;
      data& width(GLsizei );
            
      GLsizei height() const;
      data& height(GLsizei );
            
      GLenum format() const;
      data& format(GLenum );
            
      const GLvoid* ptr() const;
      data& ptr(const GLvoid*);
      
      data();

      static data depth();
      
    };

    self& send( const data& ) ;
    const data& sent() const;
    
    const self& compare() const;

  private:
    data data_;
  };


  // TODO get rid ?
  template<class Action>
  void with(const texture& t,
	    const Action& action) {
    t.bind();
    action();
    // t.unbind();
  }
  
}


#endif
