#include "bone.h"

#include <core/log.h>
#include <core/string.h>

namespace mocap {
  namespace asf {


    void bone::debug() const {
      using namespace core;
      
      log("bone:");
      
      log(indent(), "id:", id);
      log(indent(), "name:", name);
      
    }

  }
}
