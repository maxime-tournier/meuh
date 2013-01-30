#ifndef GL_PROGRAM_H
#define GL_PROGRAM_H

#include <gl/gl.h>
#include <string>

namespace gl {


  struct shader;

  struct uniform;
  struct attrib;
  
  class program {
    GLenum id_;
    typedef program self;
  public:
    program();
    
    GLenum id() const;
    
    self& gen();
    self& del();    

    const self& attach(const gl::shader& shader) const;
    
    template<class ... Args>
    const self& attach(const gl::shader& shader, Args&&... args) const {
      attach(shader); 
      return attach(std::forward<Args>(args)...);
    }
    
    const self& link() const;
  
    const self& enable() const;
    const self& disable() const;
    
    std::string log() const;
    const self& report(std::ostream& o) const;

    gl::uniform uniform(const std::string& name) const;
    gl::attrib attrib(const std::string& name) const;


    template<class Action>
    const self& draw(const Action& action) const {
      enable();
      action();
      disable();
      return *this;
    }

  };

}



#endif
