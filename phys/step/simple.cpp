#include "simple.h"

#include <phys/step/data.h>

#include <phys/solver/simple.h>
#include <phys/solver/task.h>

namespace phys {
  namespace step {

    animator_type simple(const data& d, const force_type& f) {
      return [&d, f](math::real t, math::real dt) {

	dof::velocity v;
	phys::solver::task task(v);
	
	task.momentum = d.momentum( f(t, dt), dt );
	phys::solver::simple( d.system ).solve( task );

	d.integrate(v, dt);
      };
    }
    
    animator_type simple(const system& sys, const momentum_type& p, const integrator_type& i) {
      return [&sys, p, i](math::real t, math::real dt) {
	
	dof::velocity v;
	phys::solver::task task(v);
	
	task.momentum = p(t, dt);
	phys::solver::simple( sys ).solve( task );
	
	i(v, dt);
      };
    }

  }
}


