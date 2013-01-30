#include "shader.h"

#include <iostream>

#include <core/share.h>
#include <core/io.h>
#include <core/log.h>

namespace gl {

  shader::shader(GLenum type) : id_(0), type_(type) { }

  GLenum shader::id() const { return id_; }
  GLenum shader::type() const { return type_; }
  
  shader& shader::gen() {
    id_ = glCreateShader( type() );
    return *this;
  }

  shader& shader::del() {
    glDeleteShader( id() );
    id_ = 0;
    return *this;
  }


  const shader& shader::source(const std::string& data) const {
    const char* str = data.c_str();
    int length = data.size();
    glShaderSource(id(), 1, &str, &length);
    return *this;
  }


  const shader& shader::compile() const {
    glCompileShader(id());
        
    return *this;
  }


  std::string shader::log() const {
    static const int SIZE = 500;
	  
    GLcharARB data[SIZE];
    GLsizei written = 0;
    glGetShaderInfoLog(id(), SIZE, &written, data );
    
    return std::string(data, data+written);
  }

  
  shader::vertex::vertex() : shader(GL_VERTEX_SHADER) { }
  shader::fragment::fragment() : shader(GL_FRAGMENT_SHADER) { }


  const shader& shader::report(std::ostream& out) const {
    const std::string& log = this->log();
    
    out << "shader: ";
    
    if( log.empty() ) out << "ok";
    else out << "error !" << std::endl << log;
    
    out << std::endl;
    return *this;
  }

  shader& shader::init(const std::string& filename) {
    using namespace core;

    const std::string content = io::load::ascii( share("/shader/" + filename) );
    gen().source( content ).compile();
    
    // core::log("shader", id(), "loading", filename, "...");
    
    // std::cout << "shader " <<  id() <<  " loading " <<  filename <<  "...";
    // report(std::cout);
    
    return *this;
  }
  

}
