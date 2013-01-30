#include "vbo.h"
#include <core/error.h>
#include <core/macro/here.h>

#include <core/stl.h>

namespace gl {


  


  vbo::vbo(GLenum target) 
    : id_(0), usage_( GL_STATIC_DRAW ), target_(target) { }

  GLuint vbo::id() const { return id_; }
  GLenum vbo::target() const { return target_; }
  
  vbo::self& vbo::gen() { 
    if( !id() ) { 
      glGenBuffers(1, &id_); 
      assert( id() );
    }
    return *this; 
  }
  
  vbo::self& vbo::del() {  
    if(id()) {
      glDeleteBuffers(1, &id_); }
    return *this; 
  }
   

  vbo::self& vbo::usage(GLenum u) { usage_ = u; return *this; }
  GLenum vbo::usage() const { return usage_; }

  vbo::self& vbo::draw(GLenum mode) {
    bind();
    data().layout().enable().draw( mode ).disable();
    unbind();
    return *this;
  }
        
  const vbo::data_type& vbo::data() const {
    assert( _data );
    return *_data;
  }
  
  vbo::self& vbo::send(std::unique_ptr<data_type>&& d) {
    _data = std::move( d );
    
    bind();
    glBufferData( target(),
		  data().size(),
		  data().bits(),
		  usage() );
    unbind();    
    return *this;
  }
  
  vbo::self& vbo::bind() { 
    if( !id() ) gen();
    glBindBuffer(target(), id()); 
    return *this;
  }
  
  const vbo& vbo::unbind() const { 
    glBindBuffer(target(), 0); 
    return *this;
  }

  // const vbo::layout_type& vbo::layout() const { return layout_; }
  // vbo::layout_type& vbo::layout() { return layout_; }  
  
  

  vbo::data_type::~data_type() { }

  vbo::layout_type::~layout_type() { }

  const vbo::layout_type::self& vbo::layout_type::enable() const {
    core::each(pre_draw, [](const callback& f) { f(); } );
    return *this;
  }

  const vbo::layout_type::self& vbo::layout_type::draw(GLenum mode) const {
    on_draw(mode);
    return *this;
  }
  
  const vbo::layout_type::self& vbo::layout_type::disable() const {
    core::each(post_draw, [](const callback& f) { f(); } );
    return *this;
  }
  
  unsigned int info<double>::size() { return 1; }
  GLenum info<double>::type() { return GL_DOUBLE; }
  
  GLenum info<unsigned short>::type() { return GL_UNSIGNED_SHORT; }
  unsigned int info<unsigned short>::size() { return 1; }
  
}
 
