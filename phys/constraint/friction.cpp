#include "friction.h"


#include <cassert>

namespace phys {
  namespace constraint {

    friction::friction(math::natural dim) 
      : dimension(dim) { 
      assert( dim );
    }

    math::natural friction::dim() const { return dimension; }
    
  }
}


namespace sparse {
  math::natural traits<phys::constraint::friction::key>::dimension(key_type k) { return k->dim(); }
}


