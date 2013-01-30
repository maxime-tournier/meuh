#ifndef PHYS_SOLVER_PROJECTION_H
#define PHYS_SOLVER_PROJECTION_H

#include <functional>
#include <math/types.h>

// TODO is this useful ?

namespace phys {
  namespace solver {

    typedef std::function< math::vec (const math::vec& ) > projection;
    
  }
}



#endif
