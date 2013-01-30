#ifndef MATH_FUNC_GRADIENT_H
#define MATH_FUNC_GRADIENT_H

#include <math/func.h>

#include <math/func/pull.h>

namespace math {
  namespace func {

    // TODO rename grad.h

    // more like differential (covector, no metric influence)
    template<class F>
    struct gradient {
      
      pullback<F> impl;
      
      gradient(const F& of) : impl( dT(of) ) { }
      gradient(F&& of) : impl( dT( std::move(of) ) ) { }
      
      static_assert( std::is_same< typename traits<F>::range, math::real >::value,
		     "gradient is for real functions lol" );
      
      typedef typename traits<F>::domain domain;
      typedef typename math::lie::group<domain>::coalgebra range;

      // watch out for dangling references lol
      range operator()(const domain& at) const {
	return impl( at )( 1.0 );
      }
      
    };

    template<class F>
    gradient< typename std::decay<F>::type > grad(F&& f) { return { std::forward<F>(f) }; }
    

  }
}


#endif
