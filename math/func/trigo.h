#ifndef MATH_FUNC_TRIGO_H
#define MATH_FUNC_TRIGO_H

#include <math/real.h>

namespace math {
  namespace func {
    
    // sin x / x
    real sinc( real n );

    // (cos x - 1) / x
    real cosm1c( real n);

    // real asinc( real n );   
    
  }
}

#endif
