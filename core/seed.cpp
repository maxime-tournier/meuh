#include "seed.h"

#include <core/clock.h>

namespace core {
   
  static const clock_type::time_point start = clock_type::now();
  
  int seed() {
    return (clock_type::now() - start).count();
  }

}
