#include "sqrt.h"

#include <cassert>
#include <cmath>

namespace math {
  namespace func {

    sqrt::range sqrt::operator()(const domain& x) const {
      assert( x >= 0 );
      return std::sqrt(x);
    }

    sqrt::push::push(const sqrt&, const domain& at) 
      : value( 0.5 / std::sqrt(at) ) {
      assert( at > 0 );
    }


    sqrt::range sqrt::push::operator()(const domain& x) const { 
      return value * x;
    }

  } 
}
