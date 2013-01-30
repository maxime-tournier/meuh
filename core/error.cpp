#include "error.h"

namespace core {
  error::error(const std::string& what) 
  : std::logic_error(what) { 
    assert( false );
  }
}
 
