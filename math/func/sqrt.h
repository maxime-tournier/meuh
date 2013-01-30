#ifndef MATH_FUNC_SQRT_H
#define MATH_FUNC_SQRT_H

#include <math/types.h>

namespace math {

  namespace func {

    struct sqrt {

      typedef real domain;
      typedef real range;

      range operator()(const domain& x) const;
      // T<range> operator()(const T<domain>& dx) const;

      struct push {
	const real value;
	
	push(const sqrt&, const domain& at);

	range operator()(const domain& ) const;
      };

    };


  }
}



#endif
