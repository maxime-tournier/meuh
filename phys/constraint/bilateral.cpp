#include "bilateral.h"

#include <cassert>

namespace phys {
  namespace constraint {

    bilateral::bilateral(math::natural dim) 
      : dimension( dim )
	
    { 
      assert( dim );
    }

    math::natural bilateral::dim() const { return dimension; }

  }
}

namespace sparse {


  math::natural traits<phys::constraint::bilateral::key>::dimension(key d)  { 
    return d->dim(); 
  }
  
}

