#include "data.h"

#include <sparse/prod.h>


namespace phys {
  namespace step {

    void data::integrate(const dof::velocity& v, math::real dt) const {
      engine.integrate(v, system.mass * v, dt);
    }


    dof::momentum data::momentum(const dof::force& f, math::real dt) const {
      return engine.momentum(f, dt);
    }
  

    void data::clear() { 
      engine.clear();
      system.clear();
    }
  }
}
