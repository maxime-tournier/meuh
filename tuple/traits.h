#ifndef TUPLE_TRAITS_H
#define TUPLE_TRAITS_H

#include <tuple/range.h>

namespace tuple {

  template<class Tuple> struct traits;

  template<class ... Args>
  struct traits< std::tuple<Args...> > {
    typedef typename range_type< sizeof...(Args) >::type range_type;
  };
  

}


#endif
