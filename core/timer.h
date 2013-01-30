#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include <core/clock.h>

namespace core {

  class timer {
    clock_type::time_point last;
  public:
    timer();

    // start and return elapsed time since last in seconds
    double start();
  };

  template<class Task>
  double time(const Task& task) {
    timer t;
    task();
    return t.start();
  }
  
}


#endif
