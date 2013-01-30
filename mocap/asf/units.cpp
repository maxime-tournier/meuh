#include "units.h"

#include <core/log.h>

namespace mocap {				
  namespace asf {

    void units::debug() const {
      core::log("units:");
      
      core::log("mass:", mass);

    }


  }
}
