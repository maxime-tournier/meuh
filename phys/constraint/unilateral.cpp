#include "unilateral.h"

#include <cassert>

namespace phys {
  namespace constraint {

    unilateral::unilateral(math::natural dim) 
      : dimension(dim) { 
      assert( dim );
    }

    math::natural unilateral::dim() const { return dimension; }
  }
}


namespace sparse {
  math::natural traits<phys::constraint::unilateral::key>::dimension(key_type k) { return k->dim(); }
}


