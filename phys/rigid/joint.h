#ifndef PHYS_RIGID_JOINT_H
#define PHYS_RIGID_JOINT_H


#include <math/se3.h>
#include <phys/rigid/frame.h>

namespace phys {
  namespace rigid {

    // a joint is described by two frames
    struct joint {
      rigid::frame first, second;
      // math::SE3 first, second;

      const math::SE3& frame(math::natural index) const;
    };
    
  }
}



#endif
