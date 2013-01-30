#ifndef MATH_ALGO_BOKHOVEN_H
#define MATH_ALGO_BOKHOVEN_H

#include <math/types.h>
#include <math/algo/stop.h>

#include <math/matrix.h>
#include <math/lcp.h>

namespace math {
  namespace algo {


    // see http://ioe.engin.umich.edu/people/fac/books/murty/linear_complementarity_webbook/
    // section 9.2

    // this is a kind of mix of bokhoven and gauss seidel iterations,
    // so that we don't invert M + I
    // it seems to converge
    struct bokhoven {
      
      const math::lcp& lcp;

      const vec d;
      const mat P;
      
      bokhoven(const math::lcp& lcp);
      vec solve(const vec& q, const stop& s = stop(), vec init = vec() ) const;

    };

    // solves for x>0, Ax > b through LCP
    struct bokhoven_qp {
      const math::lcp lcp;
      const bokhoven solver;
      
      bokhoven_qp(const mat& Q, const mat& A);
      
      vec solve(const vec& c, const vec& b,
		const stop& s = stop(), vec init = vec() ) const;

    };

 


  }
}



#endif
