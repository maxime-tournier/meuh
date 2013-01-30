#include "uniform.h"
#include <gl/texture.h>
#include <math/vec.h>

namespace gl {


  
  uniform& uniform::operator=(const math::vec3& v) {
    glUniform3f(location, 
		GLfloat(v.x()),
		GLfloat(v.y()),
		GLfloat(v.z()));
    return *this;
  }

  uniform& uniform::operator=(int i) {
    glUniform1i(location, i);
    return *this;
  }

  
  uniform& uniform::operator=(const texture& tex) {
    return operator=(int(tex.bind().unit_id()));
  }
}
