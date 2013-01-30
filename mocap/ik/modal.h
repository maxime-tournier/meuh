#ifndef MOCAP_IK_MODAL_H
#define MOCAP_IK_MODAL_H

#include <mocap/pose.h>
#include <mocap/ik/handle.h>
#include <math/algo/stop.h> 
#include <vector>

namespace mocap {
  namespace ik {

    struct modal {

      typedef pose domain;
      typedef pose range;

      const std::vector< handle >& handles;
      mocap::skeleton* const skeleton;
      const math::mat& modes;

      // fixes pose so that it matches constraints
      range operator()(const domain& , const math::algo::stop& = math::algo::stop() ) const;

    };

  }
}


#endif
