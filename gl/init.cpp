#include "init.h"

#include <gl/error.h>
// #include <iostream>

namespace gl {


  void init() {
    GLenum err = glewInit();
    
    if (err != GLEW_OK) {
      std::string what( "GLEW initialization failed: " );
      what = what + (const char*) glewGetErrorString(err);

      throw error(what);

    }
    
    
    glEnable( GL_NORMALIZE );


    // if (glewGetExtension("GL_EXT_framebuffer_object") == GL_TRUE) { 
    //   std::cout << "FBOs supported" << std::endl;
    // } 
  }

}
