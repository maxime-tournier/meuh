#include "get.h"

namespace gl {

  
  template<>
  bool get<bool>(GLenum pname) {
    GLboolean res;
    glGetBooleanv(pname, &res);
    return res;
  }

  template<>  
  double get<double>(GLenum pname) {
    double res;
    glGetDoublev(pname, &res);
    return res;
  }

  template<>
  float get<float>(GLenum pname) {
    float res;
    glGetFloatv(pname, &res);
    return res;
  }

  template<>
  int get<int>(GLenum pname) {
    int res;
    glGetIntegerv(pname, &res);
    return res;
  }


}
