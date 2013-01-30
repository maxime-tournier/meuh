#include "actuator.h"

namespace control {
  
  actuator::actuator( math::natural n ) : dim(n) { }

}


namespace sparse {
  namespace linear {

    math::natural dim(control::actuator::key d) { return d->dim; }

  }
}
