#ifndef MOCAP_IK_HANDLE_H
#define MOCAP_IK_HANDLE_H

#include <mocap/skeleton.h>
#include <math/se3.h>

namespace mocap {
  namespace ik {

    struct handle {
      const skeleton* joint;
      
      typedef math::SE3 value_type;
      std::function< value_type () > value; // TODO ? maybe not needed
					    // ? otherwise we need to
					    // align :-/

      handle(const skeleton* j, const std::function<value_type() >& value);
      handle();
      
    };

  }
}

#endif
