#ifndef CONTROL_OBJ_COM_H
#define CONTROL_OBJ_COM_H

#include <phys/skeleton.h>
#include <phys/skel/pose.h>
#include <phys/constraint/bilateral.h>

namespace control {
  namespace obj {

    struct com {
      const phys::constraint::bilateral constraint;
      const phys::skeleton& skeleton;
      
      math::real weight;

      com(const phys::skeleton& );

      phys::constraint::bilateral::matrix matrix(const phys::skel::pose& at ) const;
      phys::constraint::bilateral::values values(const math::T<phys::skel::pose>& dpose,
						 math::real t, 
						 math::real dt) const;
      
    };

  }
}


#endif
