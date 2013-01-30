#include "rigid.h"

#include <math/se3.h>
#include <gl/translate.h>
#include <gl/rotate.h>

namespace gl {


  void rigid(const math::SE3& data) {
    translate( data.translation() );
    rotate( data.rotation() );
  }

}
