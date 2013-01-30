#ifndef META_DEBUG_H
#define META_DEBUG_H

#include <type_traits>

namespace meta {

  template<class T> 
  struct debug {
    struct unique { };

    static_assert( std::is_same<T, unique>::value, "check");

  };
}



#endif
