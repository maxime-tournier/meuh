#include "point_size.h"
#include <gl/gl.h>

namespace gl {
 
  void point_size(math::real size){
    glPointSize( size );
  }

}
