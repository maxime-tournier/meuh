#ifndef PHYS_CONSTRAINT_FRICTION_H
#define PHYS_CONSTRAINT_FRICTION_H

#include <math/types.h>
#include <sparse/types.h>

#include <phys/dof.h>

namespace phys {
  class dof;

  namespace constraint {
    
    class unilateral;

    class friction {
      math::natural dimension;
    public:

      math::natural dim() const;
      
      friction(math::natural dim);
      
      typedef const friction* key;
      
      typedef sparse::matrix<key, typename dof::key> matrix;
      typedef sparse::vector<key> vector;
      
      typedef vector bounds;

      struct pack {
	const friction::matrix& matrix;
	const friction::bounds& bounds; // coulomb coeff ?
      };

    };
    
  }
}

namespace sparse {
  
  template<>
  struct traits< phys::constraint::friction::key > {
    typedef phys::constraint::friction::key key_type;
    static math::natural dimension(key_type d);
  };
  
}


#endif
