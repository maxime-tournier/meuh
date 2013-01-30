#ifndef MOCAP_BVH_H
#define MOCAP_BVH_H

#include <mocap/clip.h>

namespace mocap {
  
  namespace bvh {
    struct file;
    
    mocap::clip clip(const file& );
    
  }
}



#endif
