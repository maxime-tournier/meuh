#ifndef GL_SHADER_H
#define GL_SHADER_H

#include <gl/gl.h>
#include <string>

namespace gl {
  
  
  class shader {
    GLenum id_;
    const GLenum type_;

    typedef shader self;

  public:
    
    shader(GLenum type);

    GLenum id() const;
    GLenum type() const;

    self& gen();
    self& del();


    const self& source(const std::string& data) const;
    const self& compile() const;

    // writes status to stream
    const self& report(std::ostream& o) const;

    std::string log() const;
  
    struct vertex;
    struct fragment;
  
    // convenience: loads a shader under share/shader
    self& init(const std::string& filename);
    
  };

  struct shader::vertex : shader { 
    vertex();
  };
  
  struct shader::fragment : shader { 
    fragment();
  };



}


#endif
