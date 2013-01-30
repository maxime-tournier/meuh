#include "curves.h"

#include <math/func/compose.h>
#include <math/real.h>
#include <math/func/interp.h>
#include <mocap/clip.h>

#include <math/func/vector.h>

namespace mocap {

  curves::curves(const mocap::clip& clip) : clip(clip)  { }


  math::func::any<math::real, mocap::pose> curves::global() const { 
    using math::func::operator<<;
    return mocap::global(clip.skeleton.get() ) << local();
  };


  math::func::any<math::real, mocap::pose> curves::local() const { 
    const math::lie::group<mocap::pose> lie( clip.data.begin()->second.rows() );
    return math::func::curve( clip.data, lie );
  };
  

  // TODO markers

}
