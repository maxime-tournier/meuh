#ifndef MATH_FUNC_HERMITE_H
#define MATH_FUNC_HERMITE_H

#include <math/tuple.h>
#include <math/func/polynomial.h>
#include <math/func/constant.h>
#include <math/func/id.h>
#include <math/func/euclid.h> 
#include <math/func/tuple.h> 
#include <math/real.h>

#include <math/func/lambda.h>

namespace math {
  namespace func {

    namespace hermite {

      // TODO BLOAT FUUUU
      namespace { 
      auto h00 = func::lambda( [] {
	  id<real> t;
	  return 2.0 * (t^3) - 3.0 * (t^2) + val(1.0);
	});
      
      auto h10 = func::lambda( [] {
	  id<real> t;
	  return (t^3) - 2.0 * (t^2) + t;
	});
      
      auto h01 = func::lambda( [] {
	  id<real> t;
	  return  -2.0 * (t^3) + 3.0 * (t^2);
	});

      auto h11 = func::lambda( [] {
	  id<real> t;
	  return (t^3) - (t^2);
	});
      }
    }
  }
}

#endif
