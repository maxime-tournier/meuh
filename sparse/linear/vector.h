#ifndef SPARSE_LINEAR_VECTOR_H
#define SPARSE_LINEAR_VECTOR_H

#include <sparse/linear/cache.h>
#include <math/vec.h>


namespace sparse {
  namespace linear {
    
    template<class K>
    class vector {
      typedef K key_type;
      math::vec storage;
      cache<K> offset;

      typedef Eigen::Map<math::vec> value_type;
      
    public:

      vector(math::natural size = 0) {
	if(size) storage.resize(size); 
      }
      
      value_type operator()(key_type k) {
	math::natural off = offset(k);
	if( off == storage.rows() ) storage.conservativeResize(offset.size());
	
	return value_type(storage.data() + off, dim(k));
      }
      
      void clear() {
	offset.clear();
	storage.resize( 0 );	// TODO is this OK ?
      }
      
    }; 
  }
}



#endif
