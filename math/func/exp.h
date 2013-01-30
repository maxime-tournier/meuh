#ifndef MATH_FUNC_EXP_H
#define MATH_FUNC_EXP_H

#include <math/types.h>

namespace math {

  namespace func {
    
    // real exponential
    struct exp {

      typedef math::real domain;
      typedef math::real range;

      range operator()(const domain& x) const;

      struct push {
	const domain at;

	push(const exp& , const domain& at);

	range operator()(const domain& ) const;
      };
    
    };

  }
}



#endif
