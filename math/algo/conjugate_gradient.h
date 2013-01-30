#ifndef MATH_ALGO_CONJUGATE_GRADIENT_H
#define MATH_ALGO_CONJUGATE_GRADIENT_H

#include <math/vec.h>
#include <math/real.h>

#include <math/algo/iter.h>

#include <math/error.h>
#include <core/macro/here.h>
#include <cmath>

namespace math {

  namespace algo {

    namespace psd {

      struct matrix {
	const mat& original;

	matrix(const mat& original) : original(original) { 
	  if(original.rows() != original.cols() ) throw error("invalid dimensions !");
	}
	
	math::vec operator()(const math::vec& x) const {
	  if( x.rows() != int(dimension()) ) throw error("invalid vector");
	  return original * x;
	}
	
	natural dimension() const { return original.rows(); }
      };
      
    }
    


    template<class PSD = psd::matrix >
    struct conjugate_gradient {
      
      const PSD& psd;
      
      conjugate_gradient( const PSD& psd ) : psd(psd) {  }
      
      vec operator()(const vec& b, const stop& s = stop()) const {
	return operator()(b, vec::Zero(b.rows()), s); 
      }
      
      vec operator()(const vec& b, const vec& init, stop s = stop()) const {
	if( b.rows() != int(psd.dimension()) ) throw error( macro_here + "invalid rhs" );
	if( init.rows() != int(psd.dimension() ) ) throw error( macro_here + "invalid init" );
	
	// as seen on wikipedia
	vec x = init;
	
	vec r = b - psd(x);
	vec p = r;
	
	const natural n = psd.dimension();
	if( s.iterations > n ) s.iterations = n;
	
	algo::iter_eps( s, [&](real& eps) {
	    
	    const vec& Op = this->psd(p);
	    const real rk_squared = r.squaredNorm(); 
	    eps = std::sqrt( rk_squared );
	    
	    const real alpha = rk_squared / p.dot(Op);
	
	    x.noalias() += alpha * p;
	    r.noalias() -= alpha * Op;
	
	    const real beta = r.squaredNorm() / rk_squared;
	    p.noalias() = r + beta * p;
	
	  });
	
	return x;
      };

    };


    // convenience 
    template<class PSD>
    conjugate_gradient<PSD> make_cg(const PSD& psd) { return {psd}; }
    
    inline conjugate_gradient<> cg_solve(const mat& A) { return conjugate_gradient<>(A); }
    
  }
}
#endif
