#ifndef SPARSE_SUM_H
#define SPARSE_SUM_H

#include <sparse/vector.h>

namespace sparse {
 

  template<class Key>
  vector<Key> operator+(const vector<Key>& lhs, const vector<Key>& rhs) {
    const bool ordered = lhs.keys().size() < rhs.keys().size();

    vector<Key> tmp;
    
    if( ordered ) {
      tmp = rhs;
      return std::move(tmp) + lhs;
    } else {
      tmp = lhs;
      return std::move(tmp) + rhs;
    }
  }
  

  // ideally lhs is the big one
  template<class Key>
  vector<Key> operator+(vector<Key>&& lhs, const vector<Key>& rhs) {
    const bool ordered = lhs.keys().size() < rhs.keys().size();

    const vector<Key>& small = ordered ? lhs : rhs;
    
    vector<Key> res;

    if(ordered) res = rhs;
    else res = std::move(lhs);
    
    small.each([&](Key k, const_vec_chunk v) {
	
	const bool found = res.find(k, [&](vec_chunk r) {
	    r += v; 
	  });
	
	if( ! found ) res(k) = v;
	
      });
    
    return res;

  }

  template<class Key>
  vector<Key>& operator+=(vector<Key>& lhs, const vector<Key>& rhs) {
    
    rhs.each([&](Key k, const_vec_chunk v) {
	lhs(k) += v;
      });
    
    return lhs;
  }

}


#endif
