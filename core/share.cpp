#include "share.h"

#include <core/meuh.h>
#include <cassert>

namespace core {

  std::string share(const std::string& relative) {
    assert(relative[0] == '/');
    return core::meuh::root() + "/share" + relative;
  }
  
}
