#include "rest_pose.h"

#include <math/vec.h>
#include <phys/skel/joint.h>
#include <math/func/jacobian.h>

#include <core/tree/algo.h>
#include <core/macro/debug.h>

namespace control {
  namespace obj {
    
    rest_pose::rest_pose(const phys::skeleton& skeleton)
      : skeleton(skeleton) { }



    void rest_pose::set(const phys::skel::pose& pose) {
      rest_ = pose;
      
      constraint.reset( new phys::constraint::bilateral(6*(rest_.rows() - 1)) );
    }
    

    phys::constraint::bilateral::matrix rest_pose::matrix(const phys::skel::pose& at ) const {
      phys::constraint::bilateral::matrix res;
      using namespace math;

      const Lie< phys::skel::pose > G(skeleton.size);
      
      mat full = func::J( phys::skel::joint(skeleton.topology))(at).bottomRows(6 * (skeleton.size - 1));
      
      skeleton.topology->each([&](phys::skel::topology* j) {
	  const math::mat33 R = at(j->get().id).rotation().inverse().matrix();
	  
	  math::mat66 to_body;
	  to_body.block<3, 3>(0, 3) = R;
	  to_body.block<3, 3>(3, 0) = R;
	  to_body.block<3, 3>(0, 0).setZero();
	  to_body.block<3, 3>(3, 3).setZero();

	  res[constraint.get()][ j->get().dof ] = full.block(0, 6*j->get().id,
							constraint->dim, 6) * to_body;
	  
	});
      
      return res;
    }


    phys::constraint::bilateral::values rest_pose::values(const math::T<phys::skel::pose>& dpose,
							  math::real t, 
							  math::real dt) const {
      phys::constraint::bilateral::values res;

      using namespace math;
      using lie::operator*;
      const Lie<phys::skel::pose>::algebra log = lie::log( lie::inv(phys::skel::joint(skeleton.topology)(dpose.at())) * rest_);
      
      auto coords =  lie::of(dpose.at()).alg().coords(log);
      res[constraint.get()] = coords.tail(6 * (skeleton.size - 1) ) / dt;
      
      return res;
    }


  }
}

