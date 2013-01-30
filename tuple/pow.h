#ifndef TUPLE_POW_H
#define TUPLE_POW_H

#include <tuple/push.h>

namespace tuple {
   
  template<class A, int I>
  struct pow {
    typedef typename push_back< typename pow<A, I - 1>::type, A >::type type;
  };
  
  template<class A>
  struct pow<A, 0> { 
    typedef std::tuple<> type;
  };
  

}


#endif
