#ifndef PHYS_SOLVER_QP_PRECISE_H
#define PHYS_SOLVER_QP_PRECISE_H

#include <phys/solver/qp/direct.h>

namespace phys {

 
  namespace solver {

    namespace qp {

      // enforces constraint forces explicitely
      struct precise : direct {

	const math::mosek::variable lambda; // bilateral
	const math::mosek::variable mu;	    // unilateral
	const math::mosek::variable nu;	    // friction
	
	const math::mosek::constraint dynamics;
	
	// const constraint::friction::matrix C;

	precise(const phys::system& , math::real dt);
	
	dof::velocity operator()(const dof::momentum& f) const;
      };
      

    }
  }
}

#endif
