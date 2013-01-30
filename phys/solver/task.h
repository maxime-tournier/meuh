#ifndef PHYS_SOLVER_TASK_H
#define PHYS_SOLVER_TASK_H


#include <sparse/vector.h>
#include <phys/dof.h>

#include <phys/constraint/bilateral.h>
#include <phys/constraint/unilateral.h>

namespace phys {
  namespace solver {

    struct task {
      task(dof::velocity& velocity);
      
      dof::velocity& velocity;
      dof::momentum momentum;
      
      constraint::bilateral::vector bilateral;
      constraint::bilateral::vector* lambda;
    
      constraint::unilateral::vector unilateral;
      constraint::unilateral::vector* mu;

      template<class C>
      static void update(C& original, const C& with) {
	with.each([&](typename C::key_type c, sparse::const_vec_chunk v) {
	    original(c) = v;
	  });
      }
    };

  }
}


#endif
