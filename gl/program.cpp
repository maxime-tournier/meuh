#include "program.h"

#include <gl/shader.h>

#include <gl/uniform.h>
#include <gl/attrib.h>

#include <core/log.h>


namespace gl {
  program::program() : id_(0) { }
    
  GLenum program::id() const{ return id_; }
    
  program& program::gen() {
    if(!id()) {
      id_ = glCreateProgram();
    }
    return *this;
  }
  
  program& program::del() {
    if(id()) {
      glDeleteProgram( id() );
      id_ = 0;
    }
    return *this;
  }
    

  const program& program::attach(const gl::shader& shader) const {
    glAttachShader(id(), shader.id());
    return *this;
  };


  const program& program::link() const {
    glLinkProgram( id() );
    return *this;
  }
  
    
  const program& program::enable() const {
    glUseProgram(id());
    return *this;
  }

  const program& program::disable() const {
    glUseProgram(0);
    return *this;
  }

  const program& program::report(std::ostream& out) const {
    const std::string& log = this->log();
    
    if( log.empty() ) { out << "ok"; }
    else { core::log("error !"); core::log( log ); }
    
    out << std::endl;
    return *this;
  }


  std::string program::log() const {
    static const int SIZE = 500;

    GLchar data[SIZE];
    GLsizei written = 0;
    glGetProgramInfoLog(id(), SIZE, &written, data );
    
    return std::string(data, data + written);
  }

  gl::uniform program::uniform(const std::string& name) const {
    // TODO assert we're enabled
    return { glGetUniformLocation(id(), name.c_str()) };
  }

  
  gl::attrib program::attrib(const std::string& name) const {
    // TODO assert we're enabled
    return { glGetAttribLocation(id(), name.c_str()) };
  }
  
}
