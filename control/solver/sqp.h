#ifndef CONTROL_SOLVER_SQP_H
#define CONTROL_SOLVER_SQP_H


#include <control/actuator.h>
#include <control/feature.h>

#include <phys/solver/qp/precise.h>
#include <control/system.h>

#include <control/solver/numbering.h>

#include <phys/solver/affine.h>

namespace control {
  namespace solver {


    struct sqp { 

      phys::solver::qp::precise solver;
      
      const control::solver::numbering num;
       
      const math::mosek::variable act;
      const phys::solver::chunk<control::actuator> A;
      
      const math::mosek::constraint comp;
      const phys::dof::velocity v_prev;
      
      math::mat K;		// actuation compliance
      
      const math::real dt;
      const bool friction;
      const math::real laziness;
      
      mutable math::mosek::sparse_qobj qobj;
      
      void setup_actuation();

      sqp(const phys::system& phys,
	    const control::system& control,
	    const phys::dof::velocity& previous_vel,
	    math::real dt,
	    bool friction,
	    math::real laziness = 1);
      
      phys::dof::velocity operator()(const phys::dof::momentum& f,
				       control::actuator::vector&,
				       phys::dof::momentum& ) const;

    };

  }
}


#endif
