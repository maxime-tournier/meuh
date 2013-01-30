#ifndef PHYS_SOLVER_BILATERAL_H
#define PHYS_SOLVER_BILATERAL_H

#include <phys/constraint/bilateral.h>
#include <phys/dof.h>

#include <memory>

#include <phys/solver/graph.h>

#include <phys/solver/result.h>

namespace phys {
  
  struct system;
  
  namespace solver {

    struct linear_time;

    struct bilateral {
      struct pimpl_type;
      const std::unique_ptr<pimpl_type> pimpl;
      
      bilateral(const dof::mass& masses,
		const dof::mass& masses_inv,
		const constraint::bilateral::pack& pack);

      bilateral(const phys::system&);

      ~bilateral();

      dof::velocity operator()(const dof::momentum& f,
				 const result& res = result()) const;
      
      // updates the solver assuming the constraint graph topology
      // remained unchanged
      void update();

      // updates the constraint graph
      void span(const solver::graph::constraints& graph);
      
      const linear_time& acyclic() const;
      linear_time& acyclic();
      
      const solver::graph::constraints& graph() const;
      solver::graph::constraints& graph();
      
    };


    
 
  }
}

#endif
