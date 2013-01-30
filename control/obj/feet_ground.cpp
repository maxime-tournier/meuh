#include "feet_ground.h"

#include <math/func/jacobian.h>
#include <math/func/index.h>
#include <phys/skel/markers.h>
#include <core/tree/algo.h>


namespace control {
  namespace obj {
    
    using namespace math;

    feet_ground::feet_ground(const phys::skeleton& skeleton)
      : constraint( 4 ),
	skeleton(skeleton), 
	lf(0),
	rf(0),
	lt(0),
	rt(0),
	weight(1)
    {  
    }
    
    
    phys::constraint::bilateral::matrix feet_ground::matrix(const phys::skel::pose& at) const {
      assert(lf && rf && lt && rt);

      phys::constraint::bilateral::matrix res;
      auto markers = phys::skel::markers(skeleton);
      
      markers.indices.insert(lf);
      markers.indices.insert(rf);
      markers.indices.insert(lt);
      markers.indices.insert(rt);

      mat F = func::J(markers, markers.dmn(), markers.rng())(at);
      mat Fy;
      Fy.resize(constraint.dim, F.cols() );
      Fy << 
      	vec3::UnitY().transpose() * F.topRows(3),
	vec3::UnitY().transpose() * F.block(3, 0, 3, F.cols()),
	vec3::UnitY().transpose() * F.block(6, 0, 3, F.cols()),
      	vec3::UnitY().transpose() * F.bottomRows(3);
      
      math::mat full = weight * Fy;
      
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
    
    phys::constraint::bilateral::values feet_ground::values(const math::T<phys::skel::pose>& dpose,
							math::real t,
							math::real dt) const {
      phys::constraint::bilateral::values res;
      

      auto markers = phys::skel::markers(skeleton);
     
      markers.indices.insert(lf);
      markers.indices.insert(rf);
      markers.indices.insert(lt);
      markers.indices.insert(rt);
      
      auto pos = markers(dpose.at());
      
      real ground = -1;
      vec4 heights;
      heights(0) = pos(0).y();
      heights(1) = pos(1).y();
      heights(2) = pos(2).y();
      heights(3) = pos(3).y();
      
      using namespace math;

      res[&constraint] = weight  * (vec4::Constant(ground) - heights) ;
      
      return res;
    }



  }
}
