#ifndef SPARSE_UPDATE_H
#define SPARSE_UPDATE_H

#include <map>

namespace sparse {
  template<class Key, class Value, class Content>
  typename std::map<Key, Value>::iterator update(std::map<Key, Value>& into, Key k, Content&& c,
						 typename std::map<Key, Value>::iterator hint) {
    auto s = c.size();
    auto res = into.insert( hint, std::pair<Key, Value>(k, std::forward<Content>(c)) );
    
    if( s == c.size() ) {
      res->second = std::forward<Content>(c);
    }
    
    return res;    
  }
}

#endif
