#ifndef PHYS_STEP_ANIMATOR_H
#define PHYS_STEP_ANIMATOR_H

#include <functional>

#include <phys/dof.h>


namespace phys {
  namespace step {

    typedef std::function< void (math::real t, math::real dt) > animator_type;
    
  }
}


#endif
