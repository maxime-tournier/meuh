#ifndef SPARSE_INSERT_H
#define SPARSE_INSERT_H

#include <sparse/zero.h>
#include <map>


  
namespace sparse {

  // insertion facilities to handle zero insertion
  template<class Key, class Value, class Content>
  std::pair<typename std::map<Key, Value>::iterator, bool> insert(std::map<Key, Value>& into, Key k, Content&& c) {

    return into.insert( std::pair<Key, Value>(k, std::forward<Content>(c)) );
  }

  template<class Key, class Value, class Content>
  typename std::map<Key, Value>::iterator insert(std::map<Key, Value>& into, Key k, Content&& c,
						 typename std::map<Key, Value>::iterator hint) {
    return into.insert( hint, std::pair<Key, Value>(k, std::forward<Content>(c)) );
    
  }
    
  template<class Key, class Value>
  std::pair<typename std::map<Key, Value>::iterator, bool> insert(std::map<Key, Value>& into, Key, zero ) { 
    return std::make_pair(into.end(), false);
  }
    

  template<class Key, class Value>
  typename std::map<Key, Value>::iterator insert(std::map<Key, Value>& , Key, zero , 
						 typename std::map<Key, Value>::iterator it) { 
    return it;
  }
  


  template<class K1, class Content>
  void insert(std::map<K1, Content>& into,
	      const std::map<K1, Content> & what) {
    const math::natural old_size = into.size();
    into.insert(what.begin(), what.end());
    
    assert( into.size() == old_size + what.size() );
    core::unused{ old_size };
  }


}


#endif
