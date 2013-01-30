#ifndef PHYS_SOLVER_MINRES_H
#define PHYS_SOLVER_MINRES_H

#include <phys/dof.h>
#include <phys/constraint/bilateral.h>

namespace phys {

  struct system;
  struct task;
  
  namespace solver {
    
    struct task;

    struct minres {

      struct config_type {
	config_type();

	math::natural iterations;
	math::real precision;
      };

      const phys::system& system;
      
      const dof::mass& M;
      const constraint::bilateral::matrix& J;

      const math::natural m, n;
      
      config_type config;
      
      minres(const phys::system& system);
      
      void solve(const task& t) const;
      
      math::vec* warm;
    };
  }

}


#endif
