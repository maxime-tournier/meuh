#ifndef PHYS_CONTROL_OBJ_BALANCE_H
#define PHYS_CONTROL_OBJ_BALANCE_H

#include <phys/skeleton.h>
#include <phys/skel/pose.h>
#include <phys/constraint/bilateral.h>
#include <control/pd.h>

#include <set>

namespace control {

  namespace obj {
      
    struct balance {
      
      const phys::constraint::bilateral constraint;
      const phys::skeleton& skeleton;
      
      math::real height;
      mutable math::vec3 com, dir, com_body, target;

      control::pd<math::vec3> pd;

      math::natural lf, rf;

      math::real weight;
      
      balance( const phys::skeleton& skeleton);
	
      phys::constraint::bilateral::matrix matrix(const phys::skel::pose& at ) const;
      phys::constraint::bilateral::values values(const math::T<phys::skel::pose>& dpose,
						 math::real t, 
						 math::real dt) const;
      
    };

					    

  }

}

#endif
