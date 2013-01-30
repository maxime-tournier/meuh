#ifndef MATH_ITER_H
#define MATH_ITER_H

#include <math/types.h>
#include <math/real.h>

#include <core/warning.h>
#include <functional>

#include <core/debug.h>

namespace math {

  // good ol' comma trick
  namespace impl {

    template<class T>
    struct wrap {
      T value;
    };

    wrap<real> operator,(real res, wrap<real> );
    
    
  }


  struct iter {

    iter(natural bound = 0, 
	 real epsilon = 0);
    
    natural bound;
    real epsilon;

    // useful for logging
    std::function< void(natural, real) > cb;

    template<class Action> 
    natural go(const Action& action) const {
      if( !bound ) { macro_debug("zero iteration bound lol"); }
      
      const impl::wrap<real> fallback{epsilon};
      natural i;
      for(i = 0; i < bound; ++i) {
	real eps = (action(), fallback).value;
	if( eps <= epsilon ) return i + 1; 

	if( cb ) cb(i, eps);
      }
      
      return i;
    }

  };

}



#endif
