#ifndef PHYS_DOF_H
#define PHYS_DOF_H

#include <math/types.h>
#include <sparse/types.h>

namespace phys {
  class dof {
    math::natural dimension;
  public:
    
    math::natural dim() const;
    dof( math::natural n );

    typedef const dof* key;

    typedef sparse::vector<key> vector;

    typedef sparse::matrix<key, key> mass;

    typedef vector velocity;
    typedef vector momentum;
    typedef vector force;
  };
}


namespace sparse {

  template<>
  struct traits< phys::dof::key > {
    typedef phys::dof::key key;
    static math::natural dimension(key d);
  };

}

#endif
