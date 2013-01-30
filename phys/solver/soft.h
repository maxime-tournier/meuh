#ifndef PHYS_SOLVER_SOFT_H
#define PHYS_SOLVER_SOFT_H

#include <phys/system.h>
#include <phys/solver/task.h>
#include <sparse/linear.h>
#include <sparse/linear/transpose.h>
#include <sparse/linear/diagonal.h>

namespace phys {

  namespace solver {

    struct soft {
      
      const phys::system& system;

      soft(const phys::system& system);
      void solve(const solver::task& task) const;
      
    };

    
  }

}


#endif
