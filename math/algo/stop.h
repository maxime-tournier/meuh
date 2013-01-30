#ifndef MATH_ALGO_STOP_H
#define MATH_ALGO_STOP_H

#include <math/real.h>


namespace math {
  namespace algo {

    struct stop {
      real epsilon;
      natural iterations;

      stop(real epsilon = math::epsilon, natural iterations = 1000) 
	: epsilon( epsilon ), 
	  iterations( iterations )
      {  }

      stop& it(natural n ) { iterations = n; return *this; }
      stop& eps(real e ) { epsilon = e; return *this; }
      

      void debug() const;
    };

  }
}


#endif
