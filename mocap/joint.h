#ifndef MOCAP_JOINT_H
#define MOCAP_JOINT_H

#include <string>
#include <math/vec.h>
#include <ostream>


namespace mocap {
  
  struct joint {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    typedef unsigned int index;
    index id;

    std::string name;
    math::vec3 offset;
  };
  
  std::ostream& operator<<(std::ostream& o, const joint& j );
}


#endif
