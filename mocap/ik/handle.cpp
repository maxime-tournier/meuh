#include "handle.h"

namespace mocap {
  namespace ik {

    handle::handle(const skeleton* joint, 
		   const std::function<value_type() >& value)
      : joint(joint), value(value) {  }

    handle::handle() { }

  }
}
