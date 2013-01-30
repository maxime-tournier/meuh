#ifndef CORE_INSERT_H
#define CORE_INSERT_H

#include <map>

namespace core {

  // overwrites when not present
  template<class K, class V>
  V& insert(std::map<K, V>& map, const K& k, const V& value) {
    auto lb = map.lower_bound(k);

    if(lb != map.end() && !(map.key_comp()(k, lb->first)))
      {
	// key already exists
	// update lb->second if you care to
	lb->second = value;
      }
    else
      {
	// the key does not exist in the map
	// add it to the map
	map.insert(lb, MapType::value_type(k, v));    // Use lb as a hint to insert,
	// so it can avoid another lookup
      }
  }

}



#endif
