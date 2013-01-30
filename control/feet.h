#ifndef CONTROL_FEET_H
#define CONTROL_FEET_H

#include <math/types.h>
#include <math/vec.h>
#include <math/real.h>
#include <control/pd.h>

namespace control {


  struct feet {
    
    math::vec3 direction;
    math::real step_length;
    
    math::real dist;
    
    control::pd<math::vec3> pd;

    struct {
      math::vec3 left, right;
    } target;
    
    math::vec3* next;
    
    struct result_type {
      math::vec3 left, right;

    };
    
    
    result_type operator()(const math::T<math::vec3>& left,
			   const math::T<math::vec3>& right );
  };

}



#endif
