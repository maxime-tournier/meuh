#ifndef MATH_ALGO_MODULUS_H
#define MATH_ALGO_MODULUS_H

#include <math/types.h>
#include <math/algo/stop.h>

#include <math/matrix.h>
#include <math/lcp.h>

namespace math {
  namespace algo {


    // see http://ioe.engin.umich.edu/people/fac/books/murty/linear_complementarity_webbook/
    // section 9.2

    // this is a kind of mix of modulus and gauss seidel iterations,
    // so that we don't invert M + I
    // it seems to converge
    struct modulus_lcp {
      
      const mat& M;
      
      const vec d;
      const mat P;
      
      modulus_lcp(const mat& M);
      vec solve(const vec& q, const stop& s = stop() ) const;
      
    };
    
    // solves for x>0, Ax > b through LCP
    struct modulus_qp {
      const mat& Q;
      const mat& A;
      
      const vec d;
      const vec P;
      const vec H;

      modulus_qp(const mat& Q, const mat& A);
      
      vec solve(const vec& c, const vec& b, const stop& s = stop() ) const;
      
    };

 


  }
}



#endif
