#ifndef PHYS_STEP_FORCE_H
#define PHYS_STEP_FORCE_H

#include <functional>

#include <phys/dof.h>


namespace phys {
  namespace step {

    typedef std::function< dof::force (math::real t, math::real dt) > force_type;

  }
}


#endif
