#ifndef SPARSE_SPLIT_H
#define SPARSE_SPLIT_H

#include <core/exception.h>
#include <map>
#include <set>
#include <core/stl.h>

namespace sparse {

  // split map in two given keys. keys not in data correspond to
  // zero entries
  template<class K, class C>
  std::pair< std::map<K, C>, 
	     std::map<K, C> > split(const std::map<K, C>& data,
				    const std::set<K>& first ) {
    typedef std::map<K, C> map_t;
    std::pair< map_t, map_t> res;

    typename map_t::const_iterator d, de;
    typename std::set<K>::const_iterator f, fe;
      
    auto it1 = res.first.begin();
    auto it2 = res.second.begin();

    for( d = data.begin(), de = data.end(), f = first.begin(), fe = first.end();
	 d != de;) {
		
      if( (f == fe) || (d-> first < *f) )  {
	// goes into second
	it2 = ++sparse::insert(res.second, d->first, d->second, it2 ); 
	++d;
      } else if (d->first == *f ) {
	// found, goes into first
	it1 = ++sparse::insert(res.first, d->first, d->second, it1 ); 
	++d, ++f;
      } else {
	// not found in data: its zero. just skip.
	++f;
      }
	
    }

    return res;      
  }


template<class K, class C>
std::pair< std::map<K, C>, 
	   std::map<K, C> > split(std::map<K, C>&& data,
				  const std::set<K>& first ) {
  typedef std::map<K, C> map_type;
  std::pair< map_type, map_type> res;
      
  auto d = core::range::all(data);
  auto f = core::range::all(first);
      
  auto it1 = res.first.begin();
  auto it2 = res.second.begin();


  while(!d.empty()) {
    K k = d.front().first;

    if( f.empty() || k < f.front() )  {
      // goes into second
      it2 = ++sparse::insert(res.second, k, std::move(d.front().second), it2 ); 
      d.pop();
    } else if (k == f.front() ) {
      // found, goes into first
      it1 = ++sparse::insert(res.first, k, std::move(d.front().second), it1 ); 
      d.pop();
      f.pop();
    } else {
      // not found in data: its zero. just skip.
      f.pop();
    }

  }

  return res;
}




}

#endif
