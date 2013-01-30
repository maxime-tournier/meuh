#include "joint.h"

#include <stdexcept>

namespace phys {
  namespace rigid {

    const math::SE3& joint::frame(math::natural index) const {
      if( index == first.index ) return first.coords;
      if( index == second.index ) return second.coords;
      throw std::logic_error("index does not belong to joint");
    }

  }
}

