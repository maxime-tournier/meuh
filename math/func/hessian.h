#ifndef MATH_FUNC_HESSIAN_H
#define MATH_FUNC_HESSIAN_H

#include <math/func/traits.h>

namespace math {
  namespace func {

    template<class F>
    struct hessian {

      static_assert( std::is_same< typename traits<F>::range, math::real >::value,
		     "hessian is for real functions lol" );
      
      typedef typename traits<F>::domain domain;
      
      typedef typename math::lie::group<domain>::algebra algebra;
      typedef typename math::lie::group<domain>::coalgebra coalgebra;
      
      const F of;

      typename traits<F>::hess operator()(const domain& x) const {
	return {of, x};
      }
      
    };

    
    template<class F>
    hessian< typename std::decay<F>::type > H(F&& of) {
      return {std::forward<F>(of)};
    }
    
  }
}


#endif
