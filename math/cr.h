#ifndef MATH_CR_H
#define MATH_CR_H

#include <math/vec.h>
#include <math/iter.h>

namespace math {

  // cg-like, for non-singular but possibly indefinite matrices
  // TODO preconditionning ?
  template<class Matrix>
  struct conjugate_residual {
    const Matrix A;

    vec solve(const vec& b, const iter& it, vec x = vec() ) const {
       const natural n = b.rows();
            
      vec r = b;
      
      if( x.empty() ) {
	x = vec::Zero(n);
      } else {
	assert( x.rows() == n );
	r -= A(x);
      }
      
      vec p = r;
      
      vec z = A(r);
      vec w = z;
      
      real eps = b.norm();
      real mu = r.dot(z);
      
      natural k = 0;
      
      if( eps <= it.epsilon ) return x;

      it.go([&] {
	  core::log(k);
	  real w2 = w.squaredNorm();
	  assert( w2 );
	  
	  real alpha = mu / w2;
	  
	  x += alpha * p;
	  r -= alpha * w;
	  
	  eps = r.norm(); 
	  
	  z = A(r);

	  real mu_prev = mu;
	  mu = r.dot(z);	// rAr
	  
	  assert( mu_prev ); 
	  real beta = mu / mu_prev;
	  
	  p = r + beta * p;
	  w = z + beta * w;
	  
	  ++k;
	  return eps;
	});

      return x;
    }


  };


  template<class Matrix>
  conjugate_residual<Matrix> cr(const Matrix& A) { return {A}; }
  
}


#endif
