#include "modulus.h"

#include <math/lcp.h>
#include <math/algo/iter.h>

namespace math {
  namespace algo {

    
    static vec diagp1(const mat& M) {
      return M.diagonal().array() + 1;
    }
    
    
    static mat off_diag(const mat& M ) {
      mat res = M;
      res.diagonal().setZero();
      return res;
    }
    
    
    modulus_lcp::modulus_lcp(const mat& M)
      : M(M),
	d( diagp1(M) ),
	P( off_diag(M) )
    {
      
      assert( ! (d.array() == 0 ).any() );
    }
	
    vec modulus_lcp::solve(const vec& q, const stop& s) const {
      const natural n = M.rows();
      
      assert(q.rows() == int(n));
      
      vec x = vec::Zero(n);
      assert( x.rows() == q.rows() );
      
      algo::iter_eps(s, [&](real& eps) {
	  
	  x.each([&](natural i ) {
	      const real x_i = x(i);
	      
	      x(i) = (q(i) - this->P.row(i).dot(x) 
		      + std::abs( x(i) ) - this->M.row(i).dot(x.array().abs().matrix()) ) 
		/ this->d(i);
	      
	      const real diff = x(i) - x_i;
	      eps += diff * diff;
	    });
	  eps = std::sqrt(eps);
	});
      
      return (x.array().abs() + x.array());
    }


    static vec qp_diag(const mat& Q, const mat& A) {
      vec res(Q.cols() + A.rows() );
      
      res << Q.diagonal(), vec::Ones(A.rows());

      return res;
    }
    
    static mat qp_off_diag(const mat& Q, const mat& A) {
      mat res(Q.cols() + A.rows(), 
	      Q.cols() + A.rows());
      
      res << 
	off_diag(Q), -A.transpose(),
	A, mat::Zero(A.rows(), A.rows());
      
      return res;
    }
    
    static mat qp_mod(const mat& A) {
      mat res(A.rows() + A.cols(),
	      A.rows());
      
      res << A.transpose(), mat::Identity(A.rows(), A.rows()) ;
      
      return res;
    }
    
    modulus_qp::modulus_qp(const mat& Q, const mat& A) 
      : Q(Q),
	A(A),
	d(qp_diag(Q, A)),
	P(qp_off_diag(Q, A)),
	H( qp_mod(A ) )
    {
      
    }


    vec modulus_qp::solve(const vec& c, const vec& b, 
			  const stop& s ) const{
      vec q;
      q.resize( d.rows() );
      q << -c, b;
      

      vec x = vec::Zero(d.rows());
      
      algo::iter_eps(s, [&](real& eps) {
	  
	  x.each([&](natural i ) {
	      const real x_i = x(i);
	      
	      x(i) = (q(i) - this->P.row(i).dot(x) 
		      + this->H.row(i).dot(x.tail(b.rows()).array().abs().matrix()) ) 
		/ this->d(i);
	      
	      const real diff = x(i) - x_i;
	      eps += diff * diff;
	    });
	  eps = std::sqrt(eps);
	});
      
      
      return x.head(c.rows());
    }

  }
}
