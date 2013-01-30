#ifndef TOOL_RAGDOLL_H
#define TOOL_RAGDOLL_H

#include <tuple>
#include <phys/rigid/skeleton.h>

namespace tool {

  
  // ragdoll kinematics
  struct ragdoll {
    
    typedef math::vector<math::SO3> pose_type;
    typedef std::tuple< math::SO3, math::vec3 > root_type;
    typedef std::tuple< root_type, pose_type > config_type;
    
    typedef phys::rigid::config relative_type;

    typedef config_type domain;
    typedef relative_type range;
    
    math::natural root, size;
    
    math::lie::group<domain> lie() const;

    ragdoll(math::natural root, math::natural size);

    // convenience
    ragdoll(const phys::rigid::skeleton&);


    mutable range res;
    const range& operator()(const domain& x) const;

    // index map: selects internal dofs in relative parametrization
    std::vector< math::natural > index() const;

    struct push {
      math::natural root, size;
      
      domain at;
      
      push(const ragdoll& , const domain& at);

      mutable math::lie::group<range>::algebra res;
      const math::lie::group<range>::algebra& operator()(const math::lie::group<domain>::algebra& v) const;

    };

    struct pull {
      math::natural root, size;
      domain at;
      
      pull(const ragdoll& , const domain& at);

      mutable math::lie::group<domain>::coalgebra res;

      const math::lie::group<domain>::coalgebra& operator()(const math::lie::group<range>::coalgebra& f) const;

    };

    
    

  };

}


#endif
  
