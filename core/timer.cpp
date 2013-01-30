#include "timer.h"

#include <chrono>

namespace core {

  timer::timer() : last( clock_type::now() ) { }

  double timer::start() {
    using namespace std::chrono;
    
    double res = duration_cast< milliseconds > (clock_type::now() - last).count();
    
    last = clock_type::now();
    return res / 1000.0;
  }


}
 
