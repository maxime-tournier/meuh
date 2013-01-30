#include "dof.h"

#include <cassert>

namespace phys {

  dof::dof( math::natural n ) : dimension(n) { 
    assert( n );
  }
  
  math::natural dof::dim() const { return dimension; }

}


namespace sparse {
  math::natural traits<phys::dof::key>::dimension(key d) { return d->dim(); }
}

