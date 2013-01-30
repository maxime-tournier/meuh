#ifndef PHYS_RIGID_INERTIA_H
#define PHYS_RIGID_INERTIA_H

#include <math/types.h>

namespace phys {
  namespace rigid {
    namespace inertia {
      
      math::vec3 ellipsoid(const math::vec3& dimensions);
      math::vec3 cylinder(const math::vec3& dimensions);


    }
  }
}


#endif
