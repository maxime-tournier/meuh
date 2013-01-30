#ifndef SOLVER_LINEAR_TIME_H
#define SOLVER_LINEAR_TIME_H

#include <phys/constraint/bilateral.h>
#include <phys/solver/tree.h>
#include <phys/dof.h>

#include <phys/solver/result.h>

namespace phys {
  namespace solver {

    struct task;

    struct linear_time {

      // external data
      const dof::mass& masses;
      tree::constraints& tree;
      const constraint::bilateral::pack pack;
      
      struct cache_type {
	std::vector< tree::constraints::vertex_descriptor > prefix, postfix;
      };

      // internal
      const cache_type cache;
      
      linear_time( const dof::mass& masses,
		   tree::constraints& tree,
		   const constraint::bilateral::pack& pack);
      
      dof::velocity operator()(const dof::momentum& f,
				 const result& = result()) const;
      
      struct id_type {
	std::map<const phys::dof*, math::natural> dof;
	std::map<const constraint::bilateral*, math::natural> constraint;
	
	// TODO edges map ?
      } id;
      
      // updates the factorization assuming topology remained the same
      void update();

      void solve(const task& ) const;
    };
      
  }
}

#endif
