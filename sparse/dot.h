#ifndef SPARSE_DOT_H
#define SPARSE_DOT_H

#include <sparse/vector.h>

namespace sparse {

  template<class Key>
  math::real dot(const vector<Key>& lhs,
		 const vector<Key>& rhs) {
    const bool ordered = lhs.keys().size() < rhs.keys().size();

    const vector<Key>& small = ordered ? lhs : rhs;
    const vector<Key>& big = ordered ? rhs : lhs;
    
    math::real res = 0;

    small.each([&](Key k, const_vec_chunk v) {
	big.find(k, [&](const_vec_chunk b) {
	    res += v.dot(b);
	  });
      });
    
    return res;
  }

}


#endif
