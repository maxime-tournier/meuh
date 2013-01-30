#ifndef TOOL_ANGULAR_LIMITS_H
#define TOOL_ANGULAR_LIMITS_H

#include <math/lie/ellipsoid.h>
#include <math/types.h>
#include <phys/rigid/skeleton.h>
#include <map>

#include <tool/viewer.h>

namespace tool {

  struct angular_limits {
    
    typedef math::SO3 G;
    typedef math::lie::ellipsoid< G > ellipsoid;
      
    typedef std::map<math::natural, ellipsoid > result_type;
    
    const math::algo::stop outer, inner;
    const math::real fuzz;
    
    typedef phys::rigid::config frame_type;
    // typedef math::vector< frame_type > data_type;
    typedef math::vector< math::vector<math::SO3> > data_type;
    
    // compute angular limits given relative configurations
    result_type operator()(const data_type& data, math::natural root) const;
    
  };




  struct limits_viewer : viewer {
    
    angular_limits::result_type limits;
    
    math::vector< math::vector<math::SO3> > data;
    math::vector<math::SO3> current;
    
    limits_viewer();
    
    math::natural index;

    void init();
    void draw();
  };

}



#endif
