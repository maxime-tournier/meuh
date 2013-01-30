#ifndef PHYS_RIGID_COM_H
#define PHYS_RIGID_COM_H

#include <phys/rigid/skeleton.h>
#include <math/func/SE3.h>

namespace phys {
  namespace rigid {
    
    struct com {
    
      typedef config domain;
      typedef math::vec3 range;
      
      const rigid::skeleton& skeleton;
      
      com(const rigid::skeleton& );
      
      range operator()(const domain& g) const;
     
      struct push {
	push(const com&, const domain& );

	typedef math::func::vector< math::func::get_translation<> > impl_type;
	typedef math::func::traits<impl_type>::push dimpl_type;
	
	const rigid::skeleton& skeleton;
	dimpl_type dimpl;
	
	range operator()(const math::lie::group<domain>::algebra& v) const;
	
      };


      struct pull {

	pull(const com&, const domain& );

	typedef math::func::vector< math::func::get_translation<> > impl_type;
	typedef math::func::traits<impl_type>::pull dTimpl_type;
	
	const rigid::skeleton& skeleton;
	dTimpl_type dTimpl;
	
	math::lie::group<domain>::coalgebra operator()(const math::lie::group<range>::coalgebra& f) const;

      };


    };
    
    
  }
}



#endif
