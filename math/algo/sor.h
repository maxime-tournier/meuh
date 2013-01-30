#ifndef MATH_ALGO_SOR_H
#define MATH_ALGO_SOR_H

#include <math/algo/gauss_seidel.h>

namespace math {
  namespace algo {

    // successive over-relaxation: omega < 1 for better/slower
    // convergence, and vice-versa
    struct sor : gs_common {
      
      const real omega;

      sor(const mat& A, real omega) 
	: gs_common(A), omega(omega) {
	
      }
      
      template<class Projector>
      vec operator()(const vec& b, 
		     const Projector& proj = Projector(), const stop& s = stop() ) const {
	return (*this)(b, vec::Zero(b.rows()), proj, s);

      }

      template<class Projector>
      vec operator()(const vec& b, const vec& init, 
		     const Projector& proj = Projector(), const stop& s = stop() ) const {
	check(b, init);
	
	const natural n = diag.rows();
	const vec b_iter = b.array() / diag.array();
	assert(! math::nan(b_iter) );

	vec res = init;

	algo::iter_eps(s, [&](real& eps) {
	    
	    eps = 0;	
	    // Gauss-Seidel iteration
	    for(natural i = 0; i < n; ++i) {
	      const real x_i = res(i);
	  
	      res(i) = 
		(1 - omega) * res(i) +
		omega  *  ( (iter.row(i) * res)(0) + b_iter(i) ); // regular GS update
	      
	      res(i) = proj(res, i );	                 // projection
	    
	      const real diff = (res(i) - x_i);
	      eps +=  diff * diff;
	    }
	  });
	
	return res;
      }
      
    };


  }
}

#endif
