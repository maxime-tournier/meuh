#include "system.h"

#include <math/matrix.h>
#include <math/vec.h>

#include <phys/dof.h>
// #include <sparse/linear.h>


namespace control {

  
  // using sparse::linear::consistent;

  // struct clamp {

  //   math::vec operator()(const math::vec& bound, const math::vec& act) const {
  //     return act.cwiseMax(-bound).cwiseMin(bound);
  //   }

  //   sparse::zero operator()(const math::vec& , sparse::zero ) const { return {}; }
  //   sparse::zero operator()(sparse::zero, const math::vec&   ) const { return {}; }


  // };

  control::actuator::vector system::actuator_type::clamp( const control::actuator::vector& act) const {
    
    // TODO broken by sparse
    // control::actuator::vector res;
    // sparse::zip(res, bounds, act, control::clamp());

    // return res;
  }


  void system::actuator_type::clear() {
    matrix.clear();
    bounds.clear();
  }
  
  control::actuator::pack system::actuator_type::pack() const { 
    return { matrix, bounds };
  }
  


  void system::feature_type::clear() {
    storage.clear();
    matrix.clear();
    values.clear();
  }



  void system::clear() {
    feature.clear();
    actuator.clear();
  }

}
