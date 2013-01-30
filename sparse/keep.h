#ifndef SPARSE_KEEP_H
#define SPARSE_KEEP_H

#include <map>
#include <set>
#include <sparse/insert.h>
#include <core/error.h>


namespace sparse {

  // TODO rename select ?

  
  template<class K, class C, class IK>
  std::map<K, C> keep(const std::map<K, C>& data,
		      IK k, IK ke ) {
    typename std::map<K, C>::const_iterator d, de;
   
    std::map<K, C> res;
    auto it = res.end();

    if( k == ke ) return res;

    for(d = data.begin(), de = data.end();
	d != de; ++d) {
	
      if( d->first < *k ) {
	// nothing
      } else if( d->first == *k ) {
	it = insert(res, d->first, d->second, it );
	++k;
	if( k == ke ) break;
      } else {
	throw core::error("keys must be in range" );
      }

    }
      
    return res;
  }

  template<class K, class C, class IK>
  std::map<K, C> keep(const std::map<K, C>& data,
		      const std::pair<IK, IK>& range) {
    return keep(data, range.first, range.second);
  }
    
  // TODO deprecate with range( ) ohhai
  template<class K, class C> 
  std::map<K, C> keep(const std::map<K, C>& data,
		      const std::set<K>& keys ) {
    return keep(data, keys.begin(), keys.end() );
  }
    
}



#endif
