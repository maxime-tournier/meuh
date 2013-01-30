#ifndef PHYS_SOLVER_NUMBERING_H
#define PHYS_SOLVER_NUMBERING_H

#include <phys/dof.h>
#include <phys/constraint/bilateral.h>
#include <phys/constraint/unilateral.h>
#include <phys/constraint/friction.h>

#include <sparse/linear/numbering.h>

namespace phys {

  struct system;

  namespace solver {

    struct numbering {
      numbering(const system&);
      
      const sparse::linear::numbering< dof > dofs;
      const sparse::linear::numbering< constraint::bilateral > bilateral;
      const sparse::linear::numbering< constraint::unilateral > unilateral;
      const sparse::linear::numbering< constraint::friction > friction;
      
    };

  }
}

#endif
