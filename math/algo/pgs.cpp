#include "pgs.h"

#include <math/algo/iter.h>
#include <math/lcp.h>
#include <math/error.h>

#include <iostream>

namespace math {

  namespace algo {

    vec diag(const mat& original, const real eps = 0) {
      return original.diagonal() + vec::Constant(original.diagonal().rows(), eps);
    }
    
    mat iter(const math::mat& original
	    ) {
      mat tmp = original;
      tmp.diagonal().setConstant( 0 );
      
      return tmp;
    }

    pgs::pgs(const math::lcp& lcp, math::real eps ) 
      : M(lcp.M){
      // assert( ! (diag.array() == 0).any() );
    }

    pgs::pgs(const mat& M, math::real eps ) 
      : M(M) {
      assert( M.rows() == M.cols() );
      // assert( ! (diag.array() == 0).any() );

    }

    vec pgs::solve(const vec& q, const algo::stop& stop, vec res, natural offset) const {
      const natural n = q.rows();
      if( q.rows() != M.cols() ) throw error("q caca !");
      assert(offset <= n );
      
      if( res.empty() ) res = vec::Zero(n);
      if( res.rows() != M.cols() ) throw error("init caca !");
      
      algo::iter_eps(stop, [&](real& eps) {
	  eps = 0;
	  // Gauss-Seidel iteration
	  for(natural i = 0; i < n; ++i) {
	    const real x_i = res(i);
	    
	    // TODO damping / (Mii + eps)
	    const real gs = (-q(i) - M.row(i).dot(res)) / M(i, i) + x_i;
	    
	    // projection
	    res(i) = (i >= offset) ? std::max(0.0, gs) : gs;
	    
	    const real diff = res(i) - x_i;
	    eps += diff * diff;
	  }
	  
	  eps = std::sqrt(eps);
	});
      
       
      return res;
    }

  }

}
