#ifndef CONTROL_SOLVER_LOL_H
#define CONTROL_SOLVER_LOL_H



#include <phys/system.h>
#include <control/system.h>
#include <phys/solver/compliance.h>

#include <control/objective.h>

namespace control {

  namespace solver {

      struct lol {
	
	const phys::solver::compliance cache;
	const math::real epsilon;
	
	lol(const phys::system& system, math::real dt, math::real eps = 0);
	
	phys::dof::velocity operator()(const phys::dof::momentum& f,
					 control::actuator::vector& act,
					 const objective& obj) const;
	
      };




  }
}


#endif
