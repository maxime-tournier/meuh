#ifndef CORE_MACRO_FLAG_H
#define CORE_MACRO_FLAG_H

#include <core/macro/property.h>

#define macro_flag(name)			\
  macro_property(bool, name)			\
  
#endif
