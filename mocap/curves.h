#ifndef MOCAP_CURVES_H
#define MOCAP_CURVES_H

#include <math/func/any.h>
#include <mocap/pose.h>
#include <mocap/fix/time.h>

namespace mocap {

  struct clip;

  struct curves {
    
    const mocap::clip& clip;
    curves(const mocap::clip& clip);

    typedef math::func::any<time, mocap::pose> type;
    type global() const;
    type local() const;
    
    typedef math::vector<math::vec3> marker_positions;
    math::func::any<math::real, marker_positions> markers() const;

  };




}



#endif
