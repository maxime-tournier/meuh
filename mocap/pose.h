#ifndef MOCAP_POSE_H
#define MOCAP_POSE_H

#include <math/se3.h>

#include <tuple>
#include <mocap/skeleton.h> 

namespace mocap {

  typedef math::vector< math::SE3 > pose;

  // *local* tpose (translation offsets)
  pose tee_pose(mocap::skeleton* j);

  struct local {
    mocap::skeleton* skeleton;
    
    local(mocap::skeleton* skeleton) : skeleton(skeleton) { }

    typedef pose domain;
    typedef pose range;

    range operator()(const domain& p) const;
    
    // math::T<range> diff(const math::T<domain>& p) const;
  };


  struct global {
    mocap::skeleton* skeleton;
    
    typedef pose domain;
    typedef pose range;

    global(mocap::skeleton* skeleton) : skeleton(skeleton) { }
    
    range operator()(const domain& p) const;
    // math::T<range> diff(const math::T<domain>& p) const;
  };


}



#endif
