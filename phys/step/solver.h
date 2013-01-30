#ifndef PHYS_STEP_SOLVER_H
#define PHYS_STEP_SOLVER_H

#include <functional>

#include <phys/dof.h>
#include <phys/solver/any.h>

namespace phys {
  namespace step {
    
    typedef std::function< phys::solver::any () > solver_type;
    
  }
}


#endif
