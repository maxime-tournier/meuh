#ifndef MATH_LIE_SLERP_H
#define MATH_LIE_SLERP_H

#include <math/lie.h>

namespace math {
  namespace lie {

    template<class G>
    G slerp(const G& a, const G& b, const typename Lie<G>::field& alpha) {
      const Lie<G> lie(a);
      const G diff = lie.prod( lie.inv(a), b);
      return lie.prod(a, lie.exp()( lie.alg().scal(alpha, lie.log()( diff ) ) ) );
    }

  }
}



#endif
