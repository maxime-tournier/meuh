#ifndef PHYS_RIGID_BODY_H
#define PHYS_RIGID_BODY_H

#include <string>
#include <math/matrix.h>
#include <math/vec.h>

namespace phys {

  namespace rigid {

    struct body {
      
      std::string name;

      math::vec3 dim;

      math::real mass;
      math::vec3 inertia;
      
      math::mat66 tensor() const;
      
      math::real kinetic(const math::vec6& v) const;

    };

    
  }
}


#endif
