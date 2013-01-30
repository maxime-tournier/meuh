#include "body.h"

namespace phys {
  namespace rigid {

    math::mat66 body::tensor() const {
      using namespace math;
      mat66 res = mat66::Zero();
      
      res.topLeftCorner<3, 3>().diagonal() = inertia;
      res.bottomRightCorner<3, 3>().diagonal().setConstant(mass);
      
      return res;
    }


    math::real body::kinetic(const math::vec6& v) const {
      return 0.5 * ( inertia.cwiseProduct(v.head<3>()).dot(v.head<3>()) 
		     + mass * v.tail<3>().squaredNorm() );
    }
    
  }
}
