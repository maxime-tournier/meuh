#include "pack.h"
#include <mocap/bvh.h>
#include <mocap/bvh/file.h>

#include <math/func/compose.h>
#include <math/func/ref.h>

namespace mocap {
  using math::func::operator<<;

  pack::pack(const std::string& filename ) 
    : filename(filename),
      clip(mocap::bvh::clip( mocap::bvh::file(filename) ) ),
      curves(clip),
      reroot( clip.skeleton.get(), 
	      mocap::find(clip.skeleton.get(), "head end") ),
      reroot_global( math::func::ref(reroot) << curves.global() ),
      reroot_local( mocap::local(reroot.skeleton.get()) << reroot_global )
  {
    // hell yeah !
  };
  


}


