#ifndef SPARSE_MERGE_H
#define SPARSE_MERGE_H

#include <sparse/zip.h>
#include <core/error.h>

#include <algorithm>
#include <iterator>

#include <typeinfo>
#include <iostream>

namespace sparse {

  namespace helper {

    // disjoint union merge
    struct merger {
	
      template<class A>
      A&& operator()( A&& a, zero ) const { 
	return std::forward<A>(a); 
      }

      template<class A>
      A&& operator()(zero , A&& a) const { 
	// check(std::forward<A>(a));
	return std::forward<A>(a); 
      }
	
      template<class A>
      A&& operator()(A&& , A&& ) const { 
	throw core::error("can't merge duplicate keys");  
      }
	
    };

  }


  template<class K, class C>
  std::map<K, C> merge(std::map<K, C>&& a,
		       std::map<K, C>&& b) {
    std::map<K, C> res;
    sparse::zip(res, std::move(a), std::move(b), helper::merger());
    return res;
  }
    

}


#endif
