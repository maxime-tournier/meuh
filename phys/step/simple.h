#ifndef PHYS_STEP_SIMPLE_H
#define PHYS_STEP_SIMPLE_H

#include <phys/step/animator.h>
#include <phys/step/force.h>
#include <phys/step/momentum.h>
#include <phys/step/integrator.h>

namespace phys {
  struct system;
  
  namespace step {

    struct data;

    animator_type simple(const system&, const momentum_type& p, const integrator_type& i);
    animator_type simple(const data& d, const force_type& f);

  }
}


#endif
