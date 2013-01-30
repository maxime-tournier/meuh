#ifndef PHYS_SOLVER_SIMPLE_H
#define PHYS_SOLVER_SIMPLE_H

#include <phys/dof.h>
#include <phys/solver/result.h>

namespace phys {
  struct system;
  
  namespace solver {
    
    struct task;
    struct simple {

      const dof::mass& mass;
      const dof::mass& resp;
       
      simple(const dof::mass& masses, 
	     const dof::mass& resp);

      simple(const phys::system& system);

      dof::velocity operator()(const dof::momentum& f,
			       const result& res = result()) const;
  
       
      void solve(const phys::solver::task& task) const;
     
    };

    
  }
}

#endif
