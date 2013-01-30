#ifndef PHYS_CONSTRAINT_CONE_H
#define PHYS_CONSTRAINT_CONE_H

#include <math/types.h>
#include <set>

#include <phys/constraint/unilateral.h>
#include <phys/constraint/friction.h>

namespace phys {
  namespace constraint {

    class unilateral;
    class friction;

    struct cone {
      
      typedef const cone* key;

      const unilateral::key normal;
      const friction::key tangent;
      const math::real mu;

      cone(unilateral::key normal,
	   friction::key tangent,
	   math::real mu = 1.0);
      
      typedef std::set<key> set;
    };


    

  }
}


#endif
