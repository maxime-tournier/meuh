#ifndef TOOL_ADAPTOR_H
#define TOOL_ADAPTOR_H

#include <mocap/skeleton.h>

#include <phys/rigid/skeleton.h>
#include <tool/bio/info.h>

namespace tool {
  
  // builds a phys::rigid::skeleton from a mocap::skeleton
  // and maps between **absolute** poses
  struct adaptor {

    adaptor(::mocap::skeleton*, const bio::info& info);
      
    typedef std::map< ::mocap::skeleton*, math::natural> index_type;
    const index_type index;
            
    const phys::rigid::skeleton skeleton;

    // offset from joint to y-aligned, com centered frame
    const math::vector< math::SE3 > offset;

    typedef math::vector< math::SE3 > domain;
    typedef math::vector< math::SE3 > range;
      
    range operator()(const domain& ) const;
    // math::T<range> diff(const math::T<domain>& ) const;

    math::natural reverse_index(math::natural ) const;

  };


}


#endif
