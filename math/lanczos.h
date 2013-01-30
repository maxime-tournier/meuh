#ifndef MATH_LANCZOS_H
#define MATH_LANCZOS_H

#include <math/vec.h>

namespace math {
  namespace lanczos {

    // as seen in Choi2006

    struct result_type {
      real& alpha;
      real& beta;
      vec& v;
    };
    
    template<class Matrix>
    void step(const Matrix& A, 
	      const vec& v,
	      const vec& v_prev,
	      real beta,
	      real sigma,
	      result_type res)  {
      // we use res.v as work vector
      vec& p = res.v;
      
      p = A(v) - sigma * v;
      assert(!nan(p));
      
      res.alpha = v.dot( p );
      
      p -= res.alpha * v + beta * v_prev;
      
      res.beta = res.v.norm();
      
      if( res.beta ) res.v /= res.beta;
      assert( !nan(res.v) );
    }


  }
}



#endif
