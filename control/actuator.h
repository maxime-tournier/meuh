#ifndef CONTROL_ACTUATOR_H
#define CONTROL_ACTUATOR_H

#include <math/types.h>
#include <sparse/linear/dimension.h>
#include <map>
#include <phys/dof.h>

namespace control {

  struct dof;
    
  struct actuator {
    const math::natural dim;
    actuator( math::natural n );

    typedef const actuator* key;

    typedef std::map<key, math::vec> vector;

    typedef std::map<phys::dof::key, math::mat> row;
    typedef std::map<key, row> matrix;

    typedef vector bounds;

    struct pack {
      const actuator::matrix& matrix;
      const actuator::bounds& bounds;
    };

  };
  
}

namespace sparse {
  namespace linear {

    math::natural dim(control::actuator::key d);

  }
}



#endif
