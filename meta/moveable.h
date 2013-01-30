#ifndef META_MOVEABLE_H
#define META_MOVEABLE_H

#include <core/use.h>

namespace meta {
  

  template<class T>
  struct moveable {
    T value;

    moveable(const moveable& ) = delete;
    moveable(moveable&& ) = default;
    
    static moveable&& rvalue();
    
    static void check() {
      const moveable trigger = rvalue();
      core::use( trigger );
    }

  };


}


#endif
