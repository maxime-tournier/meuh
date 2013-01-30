#ifndef MOCAP_OFFSET_H
#define MOCAP_OFFSET_H


#include <mocap/skeleton.h>

namespace mocap {

  // returns unique non-zero length child offset
  math::vec3 offset(mocap::skeleton* j);
  
  // convenience
  math::real length(mocap::skeleton* j);
}



#endif
