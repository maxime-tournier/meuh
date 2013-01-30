#ifndef CONTROL_OBJECTIVE_H
#define CONTROL_OBJECTIVE_H

#include <phys/dof.h>

namespace control {

  struct objective {

    // target value should be zero
    virtual math::vec operator()(const phys::dof::velocity& v,
				 const phys::dof::momentum& a) const = 0;
    
    virtual math::natural dim() const = 0;
  
    virtual ~objective();
  
  };
  
}


#endif
