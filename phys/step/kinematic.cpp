#include "kinematic.h"

#include <phys/step/data.h>
#include <phys/solver/task.h>

namespace phys {
  namespace step {

    animator_type kinematic(const data& d, const solver_type& solver, const force_type& f) {
      return [&d,solver,f](math::real t, math::real dt) {
	
	auto s = solver();
	
	dof::velocity v;
	phys::solver::task task(v);
	
	task.momentum = d.momentum( f(t, dt), dt );

	task.bilateral = d.system.constraint.bilateral.values;
	task.unilateral = d.system.constraint.unilateral.values;
	
	s.solve( task );

	d.integrate(v, dt);
      };
    }


    animator_type kinematic(const system& sys, const solver_type& solver, const momentum_type& p, const integrator_type& i) {
      return [&sys,solver,p,i](math::real t, math::real dt) {
	
	auto s = solver();
	
	dof::velocity v;
	phys::solver::task task(v);
	
	task.momentum = p(t, dt);

	task.bilateral = sys.constraint.bilateral.values;
	task.unilateral = sys.constraint.unilateral.values;
	
	s.solve( task );

	i(v, dt);
      };

    }

    
  }
}
