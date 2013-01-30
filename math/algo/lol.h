#ifndef MATH_ALGO_LOL_H
#define MATH_ALGO_LOL_H

#include <math/mat.h>
#include <math/vec.h>
#include <math/svd.h>

namespace math {
  namespace algo {

    struct stop;

    struct lol {
      const mat Q;
      const mat A;
      
      const math::svd svd;
      const mat dec;

      const mat M;
      const vec d;
      const mat P;

      lol(const mat& Q, const mat& A);

      
      vec solve(const vec& c, const vec& b,
		const algo::stop& s) const;


    };
    

  }
}



#endif
