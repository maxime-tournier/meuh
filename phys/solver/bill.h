#ifndef PHYS_SOLVER_BILL_H
#define PHYS_SOLVER_BILL_H

#include <phys/solver/linear.h>
#include <phys/solver/ref.h>

#include <phys/solver/cholesky.h>
#include <phys/solver/cg.h>

namespace phys {
  namespace solver {
    
    struct bill {
  
      solver::linear linear;
      // typedef cg< reference<solver::linear> > full_type;
      
      typedef cholesky< reference<solver::linear> > full_type;
      const full_type full;
      
      bill(bill&&);

      bill(const phys::system& system,
	   math::real epsilon = 0);
      
      bill(const phys::system& system,
	   math::real epsilon, 
	   const constraint::bilateral::weight& w);

      void solve(const phys::solver::task& task) const;
    };

  }
}


#endif
