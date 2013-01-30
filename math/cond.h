#ifndef MATH_COND_H
#define MATH_COND_H

#include <math/types.h>

namespace math {
  
  
  real cond(const vec& eigen);
  real cond_symm(const mat& M);

}


#endif
