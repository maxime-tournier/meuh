#ifndef PHYS_SOLVER_QP_DIRECT_H
#define PHYS_SOLVER_QP_DIRECT_H

#include <phys/solver/numbering.h>
#include <math/matrix.h>

#include <math/mosek.h>
#include <plugin/mosek.h>

#include <memory>

namespace phys {

  struct system;

  namespace solver {

    namespace qp {
      
      struct direct {
	const phys::system& system;
	
	const solver::numbering num;
	
	const math::mosek::variable v;
	
	const math::mosek::constraint bilateral;
	const math::mosek::constraint unilateral;
	
	const std::unique_ptr<plugin::mosek> mosek;

	direct(const phys::system& ,
	       math::real dt);
      
	
	void correct( bool );

	dof::velocity operator()(const dof::momentum& f) const;
	
      };
   
    }
  }
}



#endif
