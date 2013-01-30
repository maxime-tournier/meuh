#include "integrator.h"

#include <phys/system.h>
#include <phys/engine.h>

#include <sparse/prod.h>

namespace phys {
  namespace step {

    integrator_type integrator(const system& sys, const engine& eng) {
      return [&](const dof::velocity& v, math::real dt) { 
	eng.integrate(v, sys.mass * v, dt); 
      };
    }
    
  }
}
