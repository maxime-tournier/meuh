#ifndef PHYS_CONSTRAINT_UNILATERAL_H
#define PHYS_CONSTRAINT_UNILATERAL_H

#include <math/types.h>
#include <sparse/types.h>

#include <phys/dof.h>

namespace phys {
  
  class dof;

  namespace constraint {

    class unilateral {
      math::natural dimension;
    public:
      math::natural dim() const;
      
      typedef const unilateral* key;

      unilateral(math::natural dim);

      typedef sparse::matrix<key, typename dof::key> matrix;
      typedef sparse::vector<key> vector;
      typedef sparse::weight<key> weight;

       struct pack {
	const unilateral::matrix& matrix;
	const unilateral::vector& values;
      };

      
    };
    
  }
}

namespace sparse {
  
  template<>
  struct traits< phys::constraint::unilateral::key > {
    typedef phys::constraint::unilateral::key key_type;
    static math::natural dimension(key_type d);
  };
  
}



#endif
