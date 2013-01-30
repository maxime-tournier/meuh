#ifndef SPARSE_DIAG_H
#define SPARSE_DIAG_H

#include <sparse/matrix.h>

namespace sparse {

  template<class Key>
  struct diagonal {
    const sparse::matrix<Key>& matrix;
    
    vector<Key> operator*(const vector<Key>& v) const {
      vector<Key> res(v.keys().size(), v.keys().dim());

      v.each([&](Key k, const_vec_chunk vk) {
	  res(k).noalias() = matrix(k, k) * vk;
	});
      
      return res;
    }


    const_mat_chunk operator()(Key d) const { return matrix(d, d); }
    
    template<class Action>
    void each(const Action& action) const {
      matrix.each([&](Key k1, Key k2, const_mat_chunk M) {
	  assert( k1 == k2 ); core::use(k2);

	  action(k1, M);
	});
    }
    
  };

  template<class Key>
  diagonal<Key> diag(const matrix<Key>& m) { return {m}; }

}


#endif
