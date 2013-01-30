#ifndef SPARSE_HADAMARD_H
#define SPARSE_HADAMARD_H

#include <sparse/vector.h>

namespace sparse {

  template<class Key>
  vector<Key> hadamard(const vector<Key>& lhs, 
		       const vector<Key>& rhs) {
    const bool ordered = lhs.keys().dim() < rhs.keys().dim();

    const vector<Key>& small = ordered ? lhs : rhs;
    const vector<Key>& big = ordered ? rhs : lhs;
    
    vector<Key> res(small.keys().size(), small.keys().dim());

    small.each([&](Key k, const_vec_chunk lv) {

	big.find(k, [&] (const_vec_chunk rv) {
	    res(k) = lv.cwiseProduct( rv );
	  });
      });
    
    return  res;
  }
  

}



#endif
