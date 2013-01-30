#ifndef CORE_CLOCK_H
#define CORE_CLOCK_H

#include <chrono>

namespace core {
  
#if __GNUC_MINOR__ <= 6
  typedef std::chrono::monotonic_clock clock_type;  // gcc 4.6
#elif __GNUC_MINOR__ >= 7
  typedef std::chrono::steady_clock clock_type;  // gcc 4.7
#endif

}

#endif
