#ifndef TOOL_MOCAP_H
#define TOOL_MOCAP_H

#include <mocap/clip.h>
#include <mocap/curves.h>
#include <tool/adaptor.h>

namespace tool {

  struct mocap {

    mocap(const std::string& filename);

    const std::string filename;	  // bvh filename
    const ::mocap::clip clip;	  // mocap clip info
    const ::mocap::curves curves; // original mocap curves
    
    const tool::adaptor adaptor; // function to adapt mocap poses to
				 // rigid::skeleton poses
    
    const phys::rigid::skeleton& skeleton() const;

    // animation curves for the rigid::skeleton
    typedef math::func::any<math::real, phys::rigid::config > curve_type;

    curve_type absolute() const;
    curve_type relative() const;
    
  };

}



#endif
