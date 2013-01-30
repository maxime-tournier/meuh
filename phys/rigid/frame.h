#ifndef PHYS_RIGID_FRAME_H
#define PHYS_RIGID_FRAME_H

#include <math/se3.h>

namespace phys {
  namespace rigid {
    
    // a frame is described by @coords wrt some rigid body with given
    // @index
    struct frame { 
      math::natural index;
      math::SE3 coords;
    };

  }
}



#endif
