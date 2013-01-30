#ifndef PHYS_SPARSE_MATRIX_H
#define PHYS_SPARSE_MATRIX_H

#include <math/matrix.h>
#include <phys/sparse/storage.h>

namespace phys {
  namespace sparse {
  
    template<int M, int N>
    struct chunk< Eigen::Matrix<math::real, M, N> > {

      typedef Eigen::Map< Eigen::Matrix<math::real, M, N> > type;
      
      // diagonal matrix
      template<class K>
      static math::natural size(K* k) {
	return k->dimension * k->dimension;
      }

      template<class K>
      static type get(K* k, const math::real* data ) {
	return type(data, k->dimension, k->dimension);
      }
      

      // regular matrix
      template<class K1, class K2>
      static math::natural size(std::pair<K1*, K2*> k) {
	return k.first->dimension * k.second->dimension;
      }

      template<class K1, class K2>
      static type get(std::pair<K1*, K2*> k, const math::real* data ) {
	return type(data, k.first->dimension, k.second->dimension);
      }
      
    };


    template<class Key>
    struct diagonal : sparse::storage<Key*, math::mat::Base> {
      // block diagonal matrix
    };
    

    template<class K1, class K2>
    struct matrix : sparse::storage< std::pair<K1*, K2*>, math::mat::Base> {
      
      // convenience
      typename matrix::block operator()(K1* k1, K2* k2) { 
    	return matrix::base::operator()(std::make_pair(k1, k2) );
      }

      const typename matrix::block operator()(K1* k1, K2* k2) const { 
    	return matrix::base::operator()(std::make_pair(k1, k2) );
      }
      
    };

  }
}

#endif
