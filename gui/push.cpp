#include "push.h"

#include <phys/unit.h>
#include <math/covector.h>
#include <core/log.h>

namespace gui {

  bool push::active() const {
    return timer.isActive() || _frames;
  }
  
  push::push() : _frames(0) { 
    timer.setSingleShot( true );
  }

  void push::go(math::real duration) {
    timer.start( duration / phys::unit::ms );
  }
  
  math::force push::force() const { return (stiffness * dir).transpose(); }
  

  void push::frames(math::natural f) { 
    core::log("pushing for", f, "frames");
    _frames = f; 
  }
  
  void push::pop() { if( _frames ) --_frames; }
  

}
