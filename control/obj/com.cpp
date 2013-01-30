#include "com.h"

#include <math/func/jacobian.h>
#include <math/func/index.h>
#include <phys/skel/com.h>
#include <core/tree/algo.h>


namespace control {
  namespace obj {
    
    using namespace math;

    com::com(const phys::skeleton& skeleton)
      : constraint( 3 ),
	skeleton(skeleton), 
	weight(1)
    {  
    }
    
    
    phys::constraint::bilateral::matrix com::matrix(const phys::skel::pose& at) const {
     
      phys::constraint::bilateral::matrix res;
         
      
      const math::mat L = func::J( phys::skel::com( skeleton ), math::lie::of(at), math::Lie< math::vec3>() )( at );
      const math::mat full = weight * L;
      
      skeleton.topology->each([&](phys::skel::topology* j) {
	  // const math::mat33 R = at(j->get().id).rotation().inverse().matrix();
	  
	  // math::mat66 to_body;
	  // to_body.block<3, 3>(0, 3) = R;
	  // to_body.block<3, 3>(3, 0) = R;
	  // to_body.block<3, 3>(0, 0).setZero();
	  // to_body.block<3, 3>(3, 3).setZero();

	  res[&constraint][ j->get().dof ] = full.block(0, 6*j->get().id,
							constraint.dim, 6) // * to_body
	    ;
	  
	});
      
      return res;
    }
    
    phys::constraint::bilateral::values com::values(const math::T<phys::skel::pose>& dpose,
							math::real t,
							math::real dt) const {
      phys::constraint::bilateral::values res;
      
      res[&constraint] = weight  * vec3::Zero();
      
      return res;
    }



  }
}
