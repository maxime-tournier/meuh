#ifndef MATH_ALGO_PGS_H
#define MATH_ALGO_PGS_H

#include <math/vec.h>
#include <math/matrix.h>
#include <math/algo/stop.h>

namespace math {
  struct lcp;

  namespace algo {


    struct pgs {

      const mat& M;

      pgs(const math::lcp& lcp, math::real eps = 0);
      pgs(const mat& M, math::real eps = 0);
      
      vec solve(const math::vec& q, 
		const algo::stop& stop = algo::stop(), 
		vec init = vec(),
		natural offset = 0) const; // only project i > offset
      
    };

  }
}
  

#endif
