#ifndef CONTROL_SOLVER_OURS_H
#define CONTROL_SOLVER_OURS_H

#include <control/actuator.h>

#include <control/feature.h>
#include <phys/solver/qp/direct.h>
#include <phys/solver/affine.h>
#include <control/feature.h>

namespace control {
  struct system;
  
  namespace solver {

    // no actuation, just plain metric tweaking
    struct ours : phys::solver::qp::direct { 
      
      const control::system& control;
      const phys::solver::chunk< control::feature > feature; 
      
      phys::dof::vector c;
      
      ours(const phys::system& phys,
	   const control::system& control,
	   math::real dt);
      
      phys::dof::velocity operator()(const phys::dof::momentum& f) const;
      
    };

  }
}


#endif
