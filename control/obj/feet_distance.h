#ifndef PHYS_CONTROL_OBJ_FEET_DISTANCE_H
#define PHYS_CONTROL_OBJ_FEET_DISTANCE_H

#include <phys/skeleton.h>
#include <phys/skel/pose.h>
#include <phys/constraint/bilateral.h>
#include <control/pd.h>

#include <set>

namespace control {

  namespace obj {
      
    struct feet_distance {
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW

      
      const phys::constraint::bilateral constraint;
      const phys::skeleton& skeleton;
      
      mutable math::real target;
      math::natural lf, rf, lt, rt;
      math::real weight;

      control::pd<math::real> pd;

      feet_distance( const phys::skeleton& skeleton);
	
      phys::constraint::bilateral::matrix matrix(const phys::skel::pose& at ) const;
      phys::constraint::bilateral::values values(const math::T<phys::skel::pose>& dpose,
						 math::real t, 
						 math::real dt) const;
      
    };

					    

  }

}

#endif
