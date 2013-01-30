#ifndef SPARSE_NORM_H
#define SPARSE_NORM_H

#include <sparse/vector.h>

namespace sparse {

  template<class Key>
  math::real norm2(const vector<Key>& v) {
    math::real res = 0.0;
    v.each([&](Key , const_vec_chunk v) {
	res += v.squaredNorm();
      });

    return res;
  }

  
  template<class Key>
  math::real norm(const vector<Key>& v) {
    return std::sqrt( norm2( v ) );
  }

}


#endif
