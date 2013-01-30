#include "any.h"

namespace phys {
  namespace solver {

    
    bool any::boolean() const { return bool(impl); }
    
    any::base::~base() { }
    

  }
}
