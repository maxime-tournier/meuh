#include "feature.h"

#include <cassert>

namespace control {

  feature::feature(math::natural dim) 
    : dim(dim) { 
    assert( dim );
  }
  
}



namespace sparse {
  namespace linear {

    math::natural dim(control::feature::key d)  { return d->dim; }
    
  }
}



