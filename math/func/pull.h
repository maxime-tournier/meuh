#ifndef MATH_FUNC_PULL_H
#define MATH_FUNC_PULL_H

#include <math/func.h>

namespace math {
  namespace func {
    
    template<class F>
    struct pullback {
      F of;
      
      pullback(const pullback& ) = default;
      pullback(pullback&& ) = default;

      typename traits<F>::pull operator()(const typename traits<F>::domain& x) const {
	return {of, x};
      }
      
    };

    template<class F>
    pullback< typename std::decay<F>::type > dT( F&& f ) { return {std::forward<F>(f)}; }
    
  }
}

#endif
