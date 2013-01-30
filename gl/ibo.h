#ifndef GL_IBO_H
#define GL_IBO_H

#warning probably broken !
#warning TODO REDO

#include <gl/error.h>
#include <vector>

namespace gl {
  




  class ibo  {
    
  public:
    static const GLenum target = GL_ELEMENT_ARRAY_BUFFER_ARB;
    
    ibo(GLenum usage);

    void bind();
    void unbind();

    virtual void enable() = 0;
    virtual void disable() = 0;

    virtual void draw(GLenum mode) = 0;

    virtual void send() = 0;
    virtual bool sent() const = 0;
    
    
  };
  
  template<class Index = unsigned short int>
  class element_array : public ibo {
    bool sent_;
  public:
    element_array(GLenum usage = buffer_object::default_usage ) 
      : ibo(usage),
	sent_(false)
    { }
    
    std::vector<Index> data;
    
    void enable() { bind();  send();  }
    void disable() {  unbind(); }
    
    bool sent() const { return sent_; }

    void send() {
      if(!sent()) {
	if( data.empty() ) throw error("no data!");
	bind();
	glBufferDataARB(target, data.size() * sizeof(Index), &data[0], usage() );
	sent_ = true;
      }
    }

    void draw(GLenum mode) {
      enable();
      glDrawElements(mode, data.size(), gl::type<Index>(), 0);
      disable();
    }
    
  };





}


#endif
