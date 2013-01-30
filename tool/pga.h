#ifndef TOOL_PGA_H
#define TOOL_PGA_H

#include <phys/rigid/skeleton.h>
#include <math/lie/pga.h>

namespace tool {
  struct mocap;
  
  struct pga {
    
    typedef math::vec domain;
    typedef math::vector< math::SO3 > range;
    
    typedef math::lie::pga< range > data_type;
    data_type data;
    

    typedef std::set< std::string > exclude_type;
    pga(const tool::mocap& mocap,
	const exclude_type& exclude = exclude_type() );
    
  };

}

#endif
