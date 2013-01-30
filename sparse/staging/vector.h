#ifndef PHYS_SPARSE_VECTOR_H
#define PHYS_SPARSE_VECTOR_H

#include <math/vec.h>
#include <phys/sparse/storage.h>

namespace phys {
  namespace sparse {

    template<int M>
    struct chunk< Eigen::Matrix<math::real, M, 1> > {
      
      template<class K>
      static math::natural size(K* k) {
	return k->dimension;
      }
      
      typedef Eigen::Map< Eigen::Matrix<math::real, M, 1 > > type;
      
      template<class K>
      static type get(K* k, const math::real* data) {
	return type(data, k->dimension);
      }
      
      
    };
    
    
    template<class Key>
    struct vector : sparse::storage<Key*, math::vec::Base > {
      
    };


  }
}


#endif
