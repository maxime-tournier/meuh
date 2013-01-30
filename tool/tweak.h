#ifndef TOOL_TWEAK_H
#define TOOL_TWEAK_H

#include <script/api.h>
#include <math/types.h>

namespace tool {
  
  // gui for tweaking lua values
  struct tweak {
    script::api api;
    
    tweak();

    // TODO add QWidget or layout ?

    void real(const std::string& lua_name,
	      math::real lower, 
	      math::real upper);
    
    // TODO moar !
    
  };

}


#endif
