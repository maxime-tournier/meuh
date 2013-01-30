#ifndef MATH_ALGO_PROG_H
#define MATH_ALGO_PROG_H

#include <math/algo/stop.h>
#include <math/vec.h>

namespace math {

  struct lcp;
  struct qp;
  
  namespace algo {
    
    vec solve(const math::lcp& lcp, const algo::stop& stop = stop());
    vec solve(const math::qp& qp, const algo::stop& stop = stop());
    
  }
}


#endif
