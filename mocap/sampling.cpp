#include "sampling.h"

#include <core/log.h>

namespace mocap {

  time sampling::time(math::natural i) const { return start + dt * i; }
  math::natural sampling::frame(mocap::time t) const {
    math::real length = frames * dt;
    
    math::real ratio = std::max(0.0, std::min(1.0, (t - start) / length));
    return std::min(frames - 1, math::natural(ratio * frames));
  }
  

}
