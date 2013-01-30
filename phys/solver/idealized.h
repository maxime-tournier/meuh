#ifndef PHYS_SOLVER_IDEALIZED_H
#define PHYS_SOLVER_IDEALIZED_H

#include <phys/constraint/bilateral.h>
#include <phys/solver/brutal.h>
#include <phys/solver/si.h>

#include <phys/solver/any.h>

namespace phys {

  namespace solver {

    struct friction;
    struct bilateral;

    // idealized friction
    struct idealized {
      const solver::friction& friction;

      const constraint::bilateral::matrix matrix;
      const constraint::bilateral::values values;
      
      constraint::bilateral::pack pack() const;

      const solver::brutal< solver::any > brutal; // ATTATION ! TODO dangling ref ?!

      // const solver::si< solver::bilateral > si;
      
      idealized(const solver::friction& , math::real eps = 0 );
      ~idealized();
      
      dof::velocity operator()(const dof::momentum& f) const;
      

    };

  }

}

#endif
