#ifndef TOOL_GL_H
#define TOOL_GL_H

#include <script/api.h>

namespace tool {

  // lua module for opengl calls
  struct gl {
    gl();
    script::api api;
  };
  
}



#endif
