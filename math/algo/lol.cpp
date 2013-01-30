#include "lol.h"

#include <math/algo/stop.h>
#include <math/algo/iter.h>


namespace math {
  namespace algo {

    vec make_d(const mat& M ) {
      return M.diagonal();
    }
    

    mat make_P(const mat& M ) {
      mat res = M;
      res.diagonal().setZero();
      return res;
    }
    
    mat make_dec(const mat& Q, const math::svd& svd) {
      std::cout << "svd rank " << svd.rank() << std::endl;

      const mat K = svd.ker();
      const mat Ap = svd.inverse();
      
      assert(K.rows() == Ap.rows());

      mat dec(K.rows(), K.cols() + Ap.cols() );
      
      if(svd.rank() ) {
	dec << K, Ap;
      } else {
	dec = Ap;
      }

      return dec;
    }


    lol::lol(const mat& Q, const mat& A) 
      : Q(Q), A(A),
	svd(A, 1e-5),
	dec( make_dec(Q, svd) ),
	M( dec.transpose() * Q * dec ),
	d( make_d(M)),
	P( make_P(M))	
    {
      assert(! math::nan(A));
      assert(! math::nan(Q));

    }

    vec lol::solve(const vec& c, const vec& b,
		    const algo::stop& s) const {
      
      vec x = vec::Zero(dec.cols());

      const vec Aib = svd.solve(b);
      
      const vec new_c = dec.transpose() * (c + Q.transpose()*Aib);
      const vec q = - new_c;
      
      const natural n = new_c.rows();
      
      iter_eps(s, [&](real& eps) {
	  eps = 0;
	  // Gauss-Seidel iteration
	  for(natural i = 0; i < n; ++i) {
	    const real x_i = x(i);
	   
	    x(i) = (q(i) - this->P.row(i).dot(x)) / this->d(i) ; // gauss-seidel
	    
	    // only project components in Im-1(A)
	    if( i >= (this->A.cols() - this->svd.rank())) { 
	      x(i) = std::max(0.0, x(i));	       // projection
	    }
	    
	    const real diff = x(i) - x_i;
	    eps += diff * diff;
	  }
	  
	  eps = std::sqrt(eps);
	});

      
      return Aib + dec * x;
    }


  }
}
