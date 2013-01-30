#ifndef PHYS_STEP_MOMENTUM_H
#define PHYS_STEP_MOMENTUM_H

#include <functional>

#include <phys/dof.h>


#include <phys/step/force.h>

namespace phys {
  class engine;
  
  namespace step {

    struct data;

    typedef std::function< dof::momentum (math::real t, math::real dt) > momentum_type;
    
    momentum_type momentum(const engine& , const force_type& );

  }
}


#endif
