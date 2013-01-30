#include "error.h"

#include <iostream>


namespace gl {

  error::error(const std::string& what) : core::error(what) { }

  GLenum error::get() {
    return glGetError();
  }

  std::string error::string(GLenum err) { 
    return (const char*)gluErrorString(err);
  }

  void error::check() {
    GLenum err = error::get();
    if( err != GL_NO_ERROR ) {
      std::cout << error::string( err ) << std::endl;
    }
  }


}
