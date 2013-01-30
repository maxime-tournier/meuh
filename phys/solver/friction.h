#ifndef PHYS_SOLVER_FRICTION_H
#define PHYS_SOLVER_FRICTION_H

#include <phys/constraint/friction.h>

#include <math/matrix.h>
#include <phys/solver/affine.h>
#include <phys/solver/any.h>

#include <math/kkt.h>


namespace math {
  namespace algo {

    struct bokhoven_qp;
    struct mosek_qp;
  }
}

namespace phys {
  namespace solver {

    struct friction {

      const solver::any bilateral;
      
      const constraint::friction::pack pack;
      const affine<phys::constraint::friction> data;
      const math::natural iter;
      
      math::mat Q;  // compliance
      mutable math::vec last;


      std::unique_ptr< math::algo::mosek_qp > qp;
      
      friction(const constraint::friction::pack& pack,
	       const solver::any& bilateral,
	       math::natural iter = 100 );
      ~friction();

      dof::velocity operator()(const dof::momentum& f, const constraint::friction::bounds& bounds) const;
      
      dof::momentum force(const dof::momentum& f, const constraint::friction::bounds& bounds) const;
      constraint::friction::vector lambda(const dof::momentum& f, const constraint::friction::bounds& bounds) const;
      
    };

  }
};


#endif
