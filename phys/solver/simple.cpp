#include "simple.h"

#include <core/range/iterator.h>
#include <sparse/diag.h>
#include <phys/solver/task.h>

#include <phys/system.h>


namespace phys {
  namespace solver {

    simple::simple(const dof::mass& mass,
		   const dof::mass& mass_inverse) 
      :	mass(mass),
	resp( mass_inverse ) { }
    
    simple::simple(const phys::system& system) : mass(system.mass),
						 resp(system.resp) { }

    void simple::solve(const phys::solver::task& task) const {
      task.velocity = sparse::diag(resp) * task.momentum;
    }
    
  }
}
