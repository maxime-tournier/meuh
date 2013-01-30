#ifndef CONTROL_OBJ_REST_POSE_H
#define CONTROL_OBJ_REST_POSE_H

#include <phys/skeleton.h>
#include <phys/skel/pose.h>
#include <phys/constraint/bilateral.h>
#include <memory>

namespace control {
  namespace obj {

    class rest_pose {
      phys::skel::pose rest_;
      std::unique_ptr<phys::constraint::bilateral> constraint;
      
    public:
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW
      
      const phys::skeleton& skeleton;

      rest_pose(const phys::skeleton& skeleton);

      // local pose
      void set(const phys::skel::pose& pose);
      

      phys::constraint::bilateral::matrix matrix(const phys::skel::pose& at ) const;
      phys::constraint::bilateral::values values(const math::T<phys::skel::pose>& dpose,
						 math::real t, 
						 math::real dt) const;
      
    };


  }
}

#endif
