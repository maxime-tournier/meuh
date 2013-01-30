#ifndef META_ENABLE_IF_H
#define META_ENABLE_IF_H

#include <meta/sfinae.h>

namespace meta {

  template<bool B, class T = enable>
  struct enable_if_bool;

  template<class T>
  struct enable_if_bool<true, T> : trigger<T> { };
  
  template<class Cond, class T = enable >
  struct enable_if : enable_if_bool< Cond::value, T > { };

  template<class Cond, class T = enable >
  struct enable_unless : enable_if_bool< !Cond::value, T >  { };

}



#endif
