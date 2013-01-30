#ifndef SPARSE_SCALAR_H
#define SPARSE_SCALAR_H

#include <sparse/vector.h>

namespace sparse {


  template<class Key>
  vector<Key> operator*(const math::real& lambda,
			const vector<Key>& v) {
    vector<Key> res(v.keys().size());
    v.each([&](Key k, const_vec_chunk x) {
	res(k).noalias() = lambda * x;
      });

    return res;
  }

  template<class Key>
  vector<Key> operator*(const vector<Key>& v, const math::real& lambda) {
    return lambda * v;
  }

  template<class Key>
  vector<Key> operator/(const vector<Key>& v, const math::real& lambda) {
    return (1.0 / lambda) * v;
  }
  

}


#endif
