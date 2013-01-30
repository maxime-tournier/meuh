#ifndef PMINRES_H
#define PMINRES_H

#include <math/pminres.h>

namespace lol {

  
  template<class Matrix, class Prec>
  struct preconditioned_minimum_residual {
    const Matrix matrix;
    const Prec prec;

    typedef math::vec vec;
    typedef math::natural natural;
    
    const std::function<void(math::natural, const vec& x)> action;

    vec solve(const vec& b, const math::iter& it, vec x = vec() ) const {
      const natural n = b.rows();
      
      math::impl::pminres data;
      
      if( x.empty() ) {
	x = vec::Zero(n);
	data.init(prec, b);
      } else {
	assert( x.rows() == +n );
	data.init(prec, b - matrix(x));
      }
      
      natural k = 0;
      it.go( [&] {
	  data.step(x, matrix, prec);
	  action(k++, x);
	  return data.r.norm(); 
	  return data.phi;
	});
      
      macro_debug("pminres sanity check:", "phi:", data.phi, "actual:", (b - matrix(x)).norm());

      return x;

    }
    
  };
  

  template<class Matrix, class Prec>
  preconditioned_minimum_residual< typename std::decay<Matrix>::type,
				   typename std::decay<Prec>::type >
  pminres(Matrix&& A, Prec&& Minv,  const  std::function<void(math::natural k, const math::vec& x)>& action) {
    return {std::forward<Matrix>(A), std::forward<Prec>(Minv), action };	
  }



}

#endif
