#ifndef PHYS_STEP_POST_STAB_H
#define PHYS_STEP_POST_STAB_H


#include <phys/step/animator.h>
#include <phys/step/force.h>
#include <phys/step/solver.h>

namespace phys {

  namespace step {
    struct data;

    animator_type post_stab(const data& d, const solver_type& s, const force_type& f);
    
  }
}


#endif

