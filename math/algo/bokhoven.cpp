#include "bokhoven.h"

#include <math/lcp.h>
#include <math/algo/iter.h>

namespace math {
  namespace algo {


    vec diagp1(const mat& M) {
      return M.diagonal().array() + 1;
    }
    
    
    mat off_diag(const mat& M ) {
      mat res = M;
      res.diagonal().setZero();
      return res;
    }
    
    
    bokhoven::bokhoven(const math::lcp& lcp)
      : lcp(lcp),
	d( diagp1(lcp.M) ),
	P( off_diag(lcp.M) )
    {
      assert( ! (d.array() == 0 ).any() );
    }
	
    vec bokhoven::solve(const vec& q, const stop& s, vec x ) const {
      const natural n = lcp.dim();
      
      assert(q.rows() == int(n));
      
      if(x.empty()) { x = vec::Zero(n); }
      assert( x.rows() == q.rows() );
      
      const mat& M = lcp.M;
      
      algo::iter_eps(s, [&](real& eps) {
	  
	  x.each([&](natural i ) {
	      const real x_i = x(i);
	      
	      x(i) = (q(i) - this->P.row(i).dot(x) + std::abs( x(i) ) - M.row(i).dot(x.array().abs().matrix()) ) / this->d(i);
	      
	      const real diff = x(i) - x_i;
	      eps += diff * diff;
	    });
	  eps = std::sqrt(eps);
	});
      
      return (x.array().abs() + x.array());
    }


    mat qp_matrix(const mat& Q, const mat& A) {
      assert(A.cols() == Q.cols());
      assert(Q.rows() == Q.cols());
      
      mat M;
      M.resize(Q.rows() + A.rows(), Q.cols() + A.rows() );

      M << Q, -A.transpose(),
	A, mat::Zero(A.rows(), A.rows() );

      return std::move(M);
    }
    
    bokhoven_qp::bokhoven_qp(const mat& Q, const mat& A) 
      : lcp( qp_matrix(Q, A)),
	solver(lcp)
    {
      
      
    }


    vec bokhoven_qp::solve(const vec& c, const vec& b, 
			   const stop& s, vec init ) const {
      assert(c.rows() + b.rows() == int(lcp.dim()));
      
      vec q;
      q.resize(lcp.dim());

      q << -c, b;
      
      return solver.solve(q, s, init );
    }

  }
}
