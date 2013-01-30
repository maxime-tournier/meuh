#include "momentum.h"

#include <phys/engine.h>

namespace phys {
  namespace step {

    momentum_type momentum(const engine& eng, const force_type& f) {
      
      return [&, f](math::real t, math::real dt) { 
	return eng.momentum(f(t, dt), dt); 
      };
      
    }

  }
}
