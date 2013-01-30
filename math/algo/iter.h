#ifndef MATH_ALGO_ITER_H
#define MATH_ALGO_ITER_H

#include <math/algo/stop.h>
#include <core/make.h>

#include <iostream>

namespace math {
  namespace algo {
    
    template<class Action>
    inline void iter(const stop& s, const Action& action) {
      for(natural i = 0; i < s.iterations; ++i ){
	action();
      }
    }
    
    
    template<class Action>
    inline stop iter_eps(stop s, const Action& action) {
      natural i = 0;
      real precision = s.epsilon;
      while( (i < s.iterations) && (precision >= s.epsilon) ) {
	action(precision);
	++i;
      }
      
      s.iterations = i;
      s.epsilon = precision;
      
      return s;
    }

    

  }
}



#endif
