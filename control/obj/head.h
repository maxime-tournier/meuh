#ifndef CONTORL_OBJ_HEAD_H
#define CONTROL_OBJ_HEAD_H

#include <phys/skeleton.h>
#include <phys/skel/pose.h>
#include <phys/constraint/bilateral.h>

#include <control/pd.h>

namespace control {
  namespace obj {

    struct head {
      
      const phys::constraint::bilateral constraint;
      const phys::skeleton& skeleton;
      
      math::real height;
      math::real weight;
      math::natural index, lf, rf;
      
      control::pd< math::vec3 > pd;
      
      head(const phys::skeleton& );

      phys::constraint::bilateral::matrix matrix(const phys::skel::pose& at ) const;
      phys::constraint::bilateral::values values(const math::T<phys::skel::pose>& dpose,
						 math::real t, 
						 math::real dt) const;
    };


  }
}


#endif
