#ifndef PHYS_RIGID_RELATIVE_H
#define PHYS_RIGID_RELATIVE_H

#include <phys/rigid/skeleton.h>

namespace phys {
  namespace rigid {

    // absolute to relative configuration
    struct relative {
      
      typedef config domain;
      typedef config range;
      
      relative(const rigid::skeleton&);
      const rigid::skeleton& skeleton;
      
      range operator()(const domain& g) const;

      struct push {
	const domain at;
	const rigid::skeleton& skeleton;
	
	push( const relative&, const domain& );
	math::lie::group<range>::algebra operator()(const math::lie::group<domain>::algebra& v) const;
	
      };
  
    };
    
  }
}



#endif
