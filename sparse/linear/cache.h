#ifndef SPARSE_LINEAR_CACHE_H
#define SPARSE_LINEAR_CACHE_H

#include <sparse/linear/dimension.h>

namespace sparse {
  namespace linear {

    // TODO rename to offset ?
    template<class K>
    class cache {
      typedef K key_type;
      typedef unsigned int value_type;
      
      typedef std::map<K, value_type> offset_type;
      offset_type offset;
      
      typedef typename offset_type::iterator iterator;
      typedef typename offset_type::const_iterator const_iterator;
      
      iterator prev;

      iterator lower_bound( key_type k ) {
	if( offset.empty() ) return offset.end();
	
	// try prev one 
	iterator res = prev;
	if(res->first == k) return res;

	// try next one
	++res;
	if((res == offset.end()) || (res->first == k)) return res;
	
	// fallback: O(log n)
	return offset.lower_bound(k);	
      }

    public:

      cache() : prev(offset.end()) { }

      void clear() { 
	offset.clear();
	prev = offset.end(); 
      }

      value_type size() const { 
	if( offset.empty() ) return 0;
	return offset.rbegin()->second + dim(offset.rbegin()->first);
      }

      value_type operator()(key_type k) {
	auto lb = lower_bound(k);
	
	math::natural off;

	if(lb != offset.end() && !(offset.key_comp()(k, lb->first))) {
	  off = lb->second;
	} else {
	  off = size();
	  lb = offset.insert(lb, std::make_pair(k, off ));    
	}
	
	prev = lb;
	return prev->second;
      }
      
    };


  }
}


#endif
