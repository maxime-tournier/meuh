#ifndef MATH_CGNE_H
#define MATH_CGNE_H

#include <math/iter.h>
#include <math/vec.h>

#include <core/log.h>
namespace math {

  template<class Matrix>	
  struct conjugate_gradient_normal_equations {

    const Matrix A;

    template<class MatrixT>
    vec solve(const vec& b, math::iter iter, const MatrixT& AT, vec x = vec()) const {

      const natural m = b.rows();
      if( x.empty() ) x = vec::Zero( m );
      
      vec r = AT( b - A(x) );
      vec p = r;
      vec Ap;

      real r_2 = r.squaredNorm();
      real eps = std::sqrt( r_2 );
      
      if( eps <= iter.epsilon ) return x;
      
      natural k = 0;
      iter.go( [&] {
	  
	  Ap = A(p);

	  real pAAp = Ap.squaredNorm();

	  if(!safe(pAAp) ) core::log("CGNE: singularity !");
	  
	  real alpha = r.squaredNorm() / pAAp;
	  
	  real old = r_2;

	  x += alpha * p;
	  r -= alpha * AT(Ap);
	  
	  r_2 = r.squaredNorm();
	  	  
	  eps = std::sqrt( r_2 );
	  
	  real beta = r_2 / old;

	  p = r + beta * p;

	  ++k;
	  return eps;
	});
      // core::log("eps:", eps);
      return x;
    }

    vec solve(const vec& b, math::iter iter) const {
      return solve(b, iter, A);
    }
    
  };

  template<class Matrix>
  conjugate_gradient_normal_equations<Matrix> cgne(const Matrix& matrix) { return {matrix}; }

}


#endif
