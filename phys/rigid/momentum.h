#ifndef PHYS_RIGID_MOMENTUM_H
#define PHYS_RIGID_MOMENTUM_H

#include <math/types.h>
#include <phys/rigid/skeleton.h>

namespace phys {
  namespace rigid {

    namespace momentum {
      
      // momentum matrix (body-fixed)
      math::matrix<math::real, 3>  angular(const rigid::skeleton&, const config& );
      math::matrix<math::real, 3>   linear(const rigid::skeleton&, const config& );
      
    }
  }
}

#endif
