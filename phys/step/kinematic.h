#ifndef PHYS_STEP_KINEMATIC_H
#define PHYS_STEP_KINEMATIC_H

#include <phys/step/animator.h>
#include <phys/step/force.h>
#include <phys/step/integrator.h>
#include <phys/step/momentum.h>
#include <phys/step/solver.h>

namespace phys {

  struct system;

  namespace step {

    struct data;

    animator_type kinematic(const data& d, const solver_type& s, const force_type& f);
    animator_type kinematic(const system& sys, const solver_type& s, const momentum_type& m, const integrator_type& i);
    
  }
}


#endif
