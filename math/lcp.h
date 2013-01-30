#ifndef MATH_LCP_H
#define MATH_LCP_H

#include <math/vec.h>

#include <math/matrix.h>

namespace math {

  // LCP in standard form: zT.(Mz + q) = 0, Mz+q >= 0, z >= 0
  struct lcp {
    const mat M;
    
    lcp(const mat&);
    lcp(mat&&);
    lcp(lcp&&);
    
    natural dim() const;
    
    
  };

}



#endif
