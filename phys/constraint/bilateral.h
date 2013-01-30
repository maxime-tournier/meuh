#ifndef PHYS_CONSTRAINT_BILATERAL_H
#define PHYS_CONSTRAINT_BILATERAL_H

#include <math/types.h>
#include <sparse/types.h>

#include <phys/dof.h>

namespace phys {


  namespace constraint {

    class bilateral {
      math::natural dimension;
    public:
      math::natural dim() const;
      
      typedef const bilateral* key;

      bilateral(math::natural dim);
      
      typedef sparse::matrix<key, typename dof::key> matrix;
      typedef sparse::vector<key> vector;
      typedef sparse::weight<key> weight;

      struct pack {
	const bilateral::matrix& matrix;
	const bilateral::vector& values;
      };

    };

  }
}

namespace sparse {
 

  template<>
  struct traits<phys::constraint::bilateral::key> {
    typedef phys::constraint::bilateral::key key;
    static math::natural dimension(key d);
  };
 
}




#endif

