#include "mean.h"

#include <mocap/clip.h>
#include <mocap/data.h>
#include <math/lie/geometric_mean.h>
#include <core/stl.h>

#include <math/func/vector.h>

namespace mocap {
  namespace algo {

    mocap::pose mean(const mocap::clip& clip) {
      using namespace math;
      const natural n = core::tree::size( clip.skeleton.get() );

      typedef mocap::pose G;

      const Lie<G> lie( n );
      
      vector< G > samples( clip.data.size() );
      
      natural off = 0;
      core::each( clip.data, [&](math::real t, const G& g) {
	  samples(off++) = g;
	});
      
      return lie::mean(lie)( samples );
    }

  }
}


