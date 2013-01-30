#ifndef CONTROL_SOLVER_CASTOR_H
#define CONTROL_SOLVER_CASTOR_H

#include <phys/system.h>
#include <control/system.h>
#include <phys/solver/compliance.h>

namespace control {
  namespace solver {


    struct castor { 

      const phys::system& phys;
      const control::system& control;
      const phys::solver::compliance cache;
      
      const phys::solver::chunk< control::feature > feature; 

      castor(const phys::system& phys,
	     const control::system& control,
	     math::real dt);

      phys::dof::velocity operator()(const phys::dof::momentum& f) const;

    };

  }
}


#endif
