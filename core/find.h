#ifndef CORE_FIND_H
#define CORE_FIND_H


// TODO customize fail with a functor

namespace core {		

  template<class K, class Cont>
  typename Cont::iterator find(Cont& cont, const K& k) {
    
    typename Cont::iterator it = cont.find(k);
    if( it == cont.end() ) throw core::error("value not found");
    return it;
  }
  
  template<class K, class Cont>
  typename Cont::const_iterator find(const Cont& cont, const K& k) {
    typename Cont::const_iterator it = cont.find(k);
    if( it == cont.end() )  throw core::error("value not found");
    return it;
  }
  
  
  

}

#endif
