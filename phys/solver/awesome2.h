#ifndef PHYS_SOLVER_AWESOME2_H
#define PHYS_SOLVER_AWESOME2_H

#include <phys/dof.h>
#include <phys/system.h>
#include <phys/solver/affine.h>
#include <phys/solver/simple.h>

#include <math/mosek.h>

namespace phys {
  namespace solver {
    
    struct awesome2 {

      const phys::system& system;

      const dof::metric obj;
      const dof::mass mass, mass_inv;
      const solver::simple response;

      const sparse::linear::numbering<phys::dof> dofs;

      const affine< constraint::bilateral > J;
      const affine< constraint::unilateral > N;
      const affine< constraint::friction > T;
      
      const affine< actuator > A;
      
      const math::natural n, a, p, q, r, k;

      math::mosek::variable v, act, lambda, normal, tangent, x, y;
      math::mosek::constraint dynamics, bilateral, unilateral, friction, cone;

      mutable math::mosek qp;

      typedef std::map< math::mosek::variable, 
			std::map< math::mosek::variable, math::mat > > compliance_type;
      compliance_type compliance;
      
      awesome2(const phys::system& system,
	      const dof::metric& obj,
	      math::real dt,
	      math::real epsilon);
      
      dof::velocity operator()(const dof::momentum& f) const;
      
      
    };



  }
}




#endif
