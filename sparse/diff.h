#ifndef SPARSE_DIFF_H
#define SPARSE_DIFF_H

#include <sparse/sum.h>

namespace sparse {
 
  template<class Key>
  vector<Key> operator-(const vector<Key>& v) {
    vector<Key> res(v.keys().size());
    
    v.each([&](Key k, const_vec_chunk v) {
	res(k).noalias() = -v;
      });
    
    return res;
  }


  template<class Key>
  vector<Key> operator-(const vector<Key>& lhs, const vector<Key>& rhs) {
    return lhs + (-rhs);
  }

  template<class Key>
  vector<Key> operator-(vector<Key>&& lhs, const vector<Key>& rhs) {
    return std::move(lhs) + (-rhs);
  }
  
  
}


#endif
