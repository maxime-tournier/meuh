#ifndef TOOL_ASF_H
#define TOOL_ASF_H

#include <phys/rigid/skeleton.h>

namespace tool {

  struct asf {
    
    asf(const std::string& name);
    
    const std::string filename;
    const phys::rigid::skeleton skeleton;
    
    
    
  };
  
}


#endif
