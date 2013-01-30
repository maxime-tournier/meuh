#ifndef SPARSE_LINEAR_DIAGONAL_H
#define SPARSE_LINEAR_DIAGONAL_H

#include <sparse/linear/func.h>
#include <sparse/linear/consistent.h>
#include <sparse/map.h>
#include <sparse/zip.h>

namespace sparse {
  namespace linear {

    // sparse diagonal matrix
    template<class Key>
    struct diagonal {
      typedef std::map<Key, math::mat> data_type;
      const data_type& data;

      diagonal(const data_type& data) : data(data) { 
	// TODO consistency check
      }

      typedef std::map<Key, math::vec> sparse_vector;
      
      sparse_vector operator*(const sparse_vector& vec) const {
	if( vec.empty() ) return sparse_vector();
	assert( consistent(vec) );
	
	sparse_vector res;
	// FIXME bug when rhs is empty ?
	zip(res, data, vec, func::product() );
	return res;
      }
      
      data_type inverse() const {
	return sparse::map<math::mat>(data, func::invert() );
      }
      
    };


    template<class Key>
    diagonal<Key> diag(const std::map<Key, math::mat>& data) {
      return { data };
    }
    
    

    // TODO improve with iter_nonzero
    template<class K1, class K2>
    std::map<K1, std::map<K2, math::mat> >
    operator*(const std::map<K1, std::map<K2, math::mat> >& lhs,
	      const diagonal<K2>& rhs) {
      
      std::map<K1, std::map<K2, math::mat> > res;
      
      core::each(lhs, [&](K1 k1, const std::map<K2, math::mat>& lrow){
	  sparse::zip_nonzero(res[k1], lrow, rhs.data, func::product() );
	});
      
      return res;
    }

  }
}


#endif
