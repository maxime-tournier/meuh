#ifndef MOCAP_ALGO_MEAN_H
#define MOCAP_ALGO_MEAN_H

#include <mocap/pose.h>

namespace mocap {
  
  struct clip;

  namespace algo {

    mocap::pose mean(const mocap::clip& clip);

  }
}




#endif
