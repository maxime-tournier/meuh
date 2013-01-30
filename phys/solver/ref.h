#ifndef PHYS_SOLVER_REF_H
#define PHYS_SOLVER_REF_H

#include <phys/dof.h>
#include <phys/solver/result.h>


namespace phys {
  
  
  namespace solver {
    struct task;

    template<class Solver>
    struct reference {
      const Solver& to;
      reference(const Solver& to) : to(to) { }
      dof::velocity operator()(const dof::momentum& f,
				 const result& res = result()) const { return to(f, res); }
    
      void solve(const solver::task& task) const { to.solve(task); }
  
    };
    
    template<class Solver>
    reference<Solver> ref(const Solver& solver) { return solver; } 

  }
}

#endif
