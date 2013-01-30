#ifndef CORE_MACRO_HERE_H
#define CORE_MACRO_HERE_H

#include <core/string.h>
 
#define HERE \
  (::std::string( __FILE__ ) + ":" + ::core::string( __LINE__ ) )

#endif
