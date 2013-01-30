#ifndef PHYS_SOLVER_STAGGERED_H
#define PHYS_SOLVER_STAGGERED_H

#include <phys/constraint/cone.h>

#include <phys/solver/any.h>

namespace phys {
  namespace solver {

    struct unilateral;
    struct friction;

    struct staggered {
      
      const solver::any bilateral;
      const solver::unilateral& unilateral;
      const solver::friction& friction;
      
      const constraint::cone::set& cones;
      
      const math::natural iter;

      staggered(const solver::any&,
		const solver::unilateral&,
		const solver::friction&,
		const constraint::cone::set& cones, 
		math::natural iter = 1);
      

      dof::velocity operator()(const dof::momentum& f) const;
      
      dof::momentum force(const dof::momentum& f) const;
      
    };
    
  }
}


#endif
