#ifndef PHYS_STEP_FAST_PROJ_H
#define PHYS_STEP_FAST_PROJ_H

#include <phys/step/animator.h>
#include <phys/step/force.h>
#include <phys/step/solver.h>

namespace phys {

  namespace step {
    struct data;

    // animator_type fast_proj(const data& d, const solver_type& s, const force_type& f, math::natural steps = 1);
    animator_type fast_proj(const data& d, const solver_type& s, const force_type& f, const math::natural& steps = 1);
 
    // runs bench on first projection
    animator_type fast_proj(const data& d, const solver_type& s, const force_type& f, const math::natural& steps,
			    const std::function<void(const phys::solver::task&) >& bench);
    
    
    
   
  }
}


#endif

