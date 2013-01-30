#ifndef CONTROL_OBJ_ANGULAR_H
#define CONTROL_OBJ_ANGULAR_H

#include <control/objective.h>
#include <phys/skeleton.h>

namespace control {
  namespace obj {
    
    struct angular : objective {
      const phys::skeleton& skeleton;
      const math::real weight;
      
      math::natural dim() const;
      math::vec operator()(const phys::dof::velocity& v,
			   const phys::actuator::vector& a) const;
      
    };


  }
}



#endif
