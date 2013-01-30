#ifndef CONTROL_SOLVER_STAGGERED_H
#define CONTROL_SOLVER_STAGGERED_H


#include <phys/actuator.h>
#include <phys/constraint/bilateral.h>

#include <phys/solver/affine.h>

#include <math/svd.h>
#include <sparse/linear.h>
#include <sparse/linear/dense.h>
#include <sparse/linear/numbering.h>

#include <math/kkt.h>

namespace phys {
  namespace solver {
    struct bilateral;
    struct staggered;
  }
}

namespace control {

  namespace solver {

    struct objective;

    struct staggered {

      const phys::solver::bilateral& bilateral;
      const phys::solver::staggered& constraint;
      const control::solver::objective& control;
      
      const math::natural iter;
      staggered( const phys::solver::bilateral& bilateral,
		 const phys::solver::staggered& constraint,
		 const control::solver::objective& control,
		 math::natural iter = 10);
      
      phys::dof::velocity operator()(const phys::dof::momentum& f) const; 
    };

  }
}


#endif
