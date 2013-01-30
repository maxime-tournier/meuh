#ifndef MATH_LIE_DISTANCE_H
#define MATH_LIE_DISTANCE_H

#include <math/lie.h>

namespace math {
  namespace lie {

    // geodesic distance
    template<class G>
    typename Euclid< typename Lie<G>::algebra >::field dist2(const G& a, const G& b) { 
      return euclid::norm2( lie::log( lie::prod(inv(a), b)));
    }

    template<class G>
    typename Euclid< typename Lie<G>::algebra >::field dist(const G& a, const G& b) { 
      return std::sqrt( dist2(a, b));
    }
      
  }
}



#endif
