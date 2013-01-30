#ifndef GL_GET_H
#define GL_GET_H

#include <gl/gl.h>

namespace gl {

  template<class Ret> Ret get(GLenum pname); 
  
  template<>
  bool get<bool>(GLenum pname);

  template<>
  double get<double>(GLenum pname);

  template<>
  float get<float>(GLenum pname);
  

  template<>
  int get<int>(GLenum pname);
  
  
}


#endif
