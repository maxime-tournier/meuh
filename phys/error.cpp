#include "error.h"

namespace phys {
 
  error::error(const std::string& what) : core::error(what) { 
    assert( false );
  }

}

