#ifndef MATH_FUNC_POLYNOMIAL_H
#define MATH_FUNC_POLYNOMIAL_H

#include <math/real.h>

#include <math/func.h>
#include <math/func/compose.h>

namespace math {
  
  namespace func {

    template<class U = real>
    struct monomial {
    
      typedef U domain;
      typedef U range;
    
      const natural degree;

      monomial(const natural& n) : degree(n) { }

      // fast exponentiation
      range operator()(const domain& x) const {
	if( degree == 0 ) return 1.0;
      	if( degree == 1 ) return x;
	
	const range res_sqrt = monomial(degree / 2)(x);
	
	if( degree % 2 == 0 ) {
	  return res_sqrt * res_sqrt;
	} else {
	  return x * res_sqrt * res_sqrt;
	}
	
      };

      struct push {
	const U value;

	push(const monomial& of, const domain& at) 
	  : value( of.degree * monomial(of.degree - 1)(at) ) {

	}
	
	U operator()(const U& dx) const {
	  return value * dx;
	}

      };
      
      // TODO pullback
    
    };


    template<class F>
    compose< monomial<typename F::range>, F > operator^(const F& f, natural n) {
      return monomial<typename F::range>(n) << f;
    }
    
  }
}

#endif
