#include "cone.h"

#include <cassert>

namespace phys {
  namespace constraint {
    
    cone::cone(unilateral::key normal,
	       friction::key tangent,
	       math::real mu) 
      : normal(normal), tangent(tangent), mu(mu) 
    {
      assert(mu > 0);
    }

    
  }
}
