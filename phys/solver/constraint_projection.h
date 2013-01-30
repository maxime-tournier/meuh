#ifndef PHYS_SOLVER_CONSTRAINT_PROJECTION_H
#define PHYS_SOLVER_CONSTRAINT_PROJECTION_H

#include <phys/solver/projection.h>

namespace phys {
  struct constraint;

  // TODO move up ?
  namespace solver {

    typedef std::map<const constraint*, projection > constraint_projection;
    
  }
}


#endif
