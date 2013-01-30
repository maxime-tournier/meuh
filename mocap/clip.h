#ifndef MOCAP_CLIP_H
#define MOCAP_CLIP_H

#include <mocap/skeleton.h>
#include <mocap/data.h>
#include <mocap/sampling.h>

#include <memory>

namespace mocap {
  
  struct clip {
    std::unique_ptr<mocap::skeleton> skeleton;

    mocap::data data;
    mocap::sampling sampling;

    clip() = default;
    clip(clip&& ) = default;
    clip& operator=(clip&& other);


    // hack
    math::matrix< math::vec3 > angles;
    
  };

}



#endif
