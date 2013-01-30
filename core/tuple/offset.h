#ifndef CORE_TUPLE_OFFSET_H
#define CORE_TUPLE_OFFSET_H

#include <tuple>

namespace core {
  namespace tuple {
    
    // returns the offset of the ith element in a tuple
    // beware: elements are actually stored in opposite order !
    template<int I, class ... Args> struct offset;
    
    template<class Head, class H, class ... Tail>
    struct offset<0, Head, H, Tail...> { 
      static const unsigned int value = 
	offset<0, H, Tail...>::value + sizeof(H);
    };

    template<class Head>
    struct offset<0, Head> { 
      static const unsigned int value = 0;
    };
    
    template<int I, class Head, class ... Tail>
    struct offset<I, Head, Tail...> : offset<I-1, Tail...> {
    };
    
    
  }
}


#endif
