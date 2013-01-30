#ifndef CONTROL_OBJ_FEET_GROUND_H
#define CONTROL_OBJ_FEET_GROUND_H

#include <phys/skeleton.h>
#include <phys/skel/pose.h>
#include <phys/constraint/bilateral.h>

namespace control {
  namespace obj {

    struct feet_ground {
      const phys::constraint::bilateral constraint;
      const phys::skeleton& skeleton;
      math::natural lf, rf, lt, rt;

      math::real weight;

      feet_ground(const phys::skeleton& );

      phys::constraint::bilateral::matrix matrix(const phys::skel::pose& at ) const;
      phys::constraint::bilateral::values values(const math::T<phys::skel::pose>& dpose,
						 math::real t, 
						 math::real dt) const;
      
    };

  }
}


#endif
