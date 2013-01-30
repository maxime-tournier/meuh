#ifndef SPARSE_DENSE_H
#define SPARSE_DENSE_H

#include <sparse/offset.h>
#include <sparse/vector.h>

namespace sparse {

  template<class Key, class Out>
  void dense(Out&& out, const offset<Key>& off,
	     const vector<Key>& data) {
    out = math::vec::Zero( off.dim() );
    
    off.each([&](Key k, math::natural o) {
	data.find(k, [&](const_vec_chunk v) {
	    math::natural d = dim(k);
	    out.segment(o, d) = v;
	  });
      });
    
  }

  
  template<class Key>
  math::vec dense(const offset<Key>& off,
  		  const vector<Key>& data) {
    math::vec res;
    dense(res, off, data);
    return res;
  }

  
  template<class Key, class Data>
  vector<Key> dense(const offset<Key>& off,
		    const Data& data) {
    return vector<Key>(off, data);
  }
  

 
  


}



#endif
