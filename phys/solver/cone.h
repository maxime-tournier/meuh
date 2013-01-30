#ifndef SOLVER_CONE_H
#define SOLVER_CONE_H

#include <math/space/real.h>
#include <math/space/matrix.h>

namespace phys {
  namespace solver {


    struct cone {
      const math::vec3 normal;
      const math::real mu;
      
      math::mat33 basis() const;

      math::vec project(const math::vec& ) const;
      
    };

    
        
  }
}

#endif
