#ifndef MATH_PGS_H
#define MATH_PGS_H

#include <math/mat.h>
#include <math/vec.h>
#include <math/iter.h>

namespace math {


  struct pgs {
    mat M;

    vec solve(const vec& q, iter it) const;
    
  };


}


#endif
