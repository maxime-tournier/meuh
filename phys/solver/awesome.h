#ifndef PHYS_SOLVER_AWESOME_H
#define PHYS_SOLVER_AWESOME_H

#include <phys/dof.h>
#include <phys/system.h>
#include <phys/solver/affine.h>
#include <phys/solver/simple.h>

#include <math/algo/pgs.h>

namespace phys {
  namespace solver {
    struct awesome {

      const phys::system& system;
      const dof::metric obj;

      const struct part {
	const dof::mass matrix, matrix_inv;
	const solver::simple solver;
	
	const affine< phys::constraint::bilateral > bilateral;
	const affine< phys::constraint::unilateral > unilateral;
	
	part(const dof::mass&, 
	     const system::constraint_type& );
	
      } integration, metric;

      const math::mat compliance;

      typedef math::algo::pgs inv_type;
      const std::unique_ptr<inv_type> inv;
      
      awesome(const phys::system& system,
	      const dof::metric& obj,
	      math::real dt,
	      math::real epsilon);
      
      dof::velocity operator()(const dof::momentum& f) const;

      
    };



  }
}




#endif
