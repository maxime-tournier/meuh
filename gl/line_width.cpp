#include "line_width.h"
#include <gl/gl.h>

namespace gl {

  void line_width(math::real width){
    glLineWidth( width );
  }

}
