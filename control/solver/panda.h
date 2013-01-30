#ifndef CONTROL_SOLVER_PANDA_H
#define CONTROL_SOLVER_PANDA_H


#include <control/actuator.h>
#include <control/feature.h>

#include <phys/solver/qp/precise.h>
#include <control/system.h>

#include <control/solver/numbering.h>

#include <phys/solver/affine.h>

namespace control {
  namespace solver {


    struct panda { 

      phys::solver::qp::precise solver;
      phys::solver::qp::direct final;
      
      const control::solver::numbering num;
       
      const math::mosek::variable act;
      const phys::solver::chunk<control::actuator> A;
      
      const math::mosek::constraint comp;
      math::mat K;

      const math::real dt;
      const bool relax;
      
      const math::real laziness;
      const math::real reflexes;


      phys::dof::vector c;
      std::function< void( const phys::dof::momentum& ) > setup;

      panda(const phys::system& phys,
	    const control::system& control,
	    const phys::dof::velocity& previous_vel,
	    math::real dt,
	    bool relax,
	    math::real laziness = 1,
	    math::real reflexes = 0);
      

      phys::dof::velocity operator()(const phys::dof::momentum& f,
				       control::actuator::vector&) const;

    };

  }
}


#endif
