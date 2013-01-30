#ifndef SPARSE_KEYS_H
#define SPARSE_KEYS_H

#include <core/range/iterator.h>
#include <core/range/pair.h>

namespace sparse {
  
  template<class K, class C>
  core::range::first< core::range::iterator< typename std::map<K*, C>::const_iterator > >
  keys(const std::map<K*, C>& cont) {
    return core::range::take_first( core::range::all(cont) );
  }
  
}


#endif
