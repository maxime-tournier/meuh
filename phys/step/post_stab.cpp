#include "post_stab.h"

#include <phys/dof.h>
#include <phys/solver/task.h>
#include <phys/step/data.h>

namespace phys {
  namespace step {

    animator_type post_stab(const data& d, const solver_type& solver, const force_type& f) {
      return [&d,solver,f](math::real t, math::real dt) {
	
	auto s = solver();
	
	dof::velocity v;
	phys::solver::task task(v);
	
	task.momentum = d.momentum( f(t, dt), dt );

	task.bilateral = d.system.constraint.bilateral.values;
	task.unilateral = d.system.constraint.unilateral.values;
	
	s.solve( task );
	d.integrate(v, dt);

	task.momentum.clear();
	task.bilateral = d.system.constraint.bilateral.corrections;
	// task.unilateral = d.system.constraint.unilateral.corrections;
	
	s.solve( task );
	d.engine.correct( v );
	
      };
    }

  }
}
