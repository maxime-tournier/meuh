#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include <core/log.h>
#include <core/macro/here.h>

#ifndef NDEBUG
#define macro_debug(...) \
  core::log(HERE, ":",__VA_ARGS__)
#else
#define macro_debug(...)			\
  
#endif

#endif
