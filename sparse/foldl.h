#ifndef SPARSE_FOLDL_H
#define SPARSE_FOLDL_H

#include <map>

namespace sparse {

  template<class Init, class Key, class Content, class F>
  Init foldl(const std::map<Key, Content>& m, Init res, const F& f = F() ) {
    
    typedef typename std::map<Key, Content>::const_iterator it;
    
    for( it i = m.begin(), e = m.end(); i != e; ++i) {
      res = f( res, i->second);
    }
    
    return res;
  }

}

#endif
