#include "staggered.h"

#include <phys/solver/staggered.h>
#include <phys/solver/bilateral.h>

#include <control/solver/objective.h>

#include <iostream>

namespace control {
  namespace solver {


    staggered::staggered( const phys::solver::bilateral& bilateral,
			  const phys::solver::staggered& constraint,
			  const control::solver::objective& control,
			  math::natural iter)
      : bilateral(bilateral),
	constraint(constraint),
	control(control),
	iter(iter) {

    }
  
    phys::dof::velocity staggered::operator()(const phys::dof::momentum& f) const {
      
      phys::dof::momentum constraints, actuation;
      using sparse::linear::operator+;

      for(math::natural i = 0; i < iter; ++i ) {
	actuation = control.force( f + constraints );
	constraints = constraint.force(f + actuation);
      }
      
      return bilateral( f + actuation + constraints );
    }
      
  }
}
