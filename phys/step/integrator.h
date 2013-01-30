#ifndef PHYS_STEP_INTEGRATOR_H
#define PHYS_STEP_INTEGRATOR_H

#include <functional>

#include <phys/dof.h>


namespace phys {

  struct system;
  class engine;

  namespace step {

    typedef std::function< void(const dof::velocity& v, math::real dt) > integrator_type;

    integrator_type integrator(const system& sys, const engine& eng);

  }
}


#endif
