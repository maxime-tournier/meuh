#include "exp.h"

#include <cmath>

#include <math/real.h>

namespace math {

  namespace func {
    

    exp::range exp::operator()(const domain& x) const {
      return math::epsilon + std::exp( x );
    }

    exp::push::push(const exp& , const domain& at) 
      : at( math::epsilon + std::exp( at ) ) { }
    
    exp::range exp::push::operator()(const domain& body) const {
      return at * body;
    }
    
  }
}
