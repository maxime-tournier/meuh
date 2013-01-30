#ifndef PHYS_STEP_DATA_H
#define PHYS_STEP_DATA_H

#include <phys/system.h>
#include <phys/engine.h>

namespace phys {
  namespace step {

    struct data {
      
      phys::system system;
      phys::engine engine;
      
      void clear();

      void integrate(const dof::velocity&, math::real dt) const;
      dof::momentum momentum(const dof::force&, math::real dt) const;
      
    };



  }
}
		


#endif
