#include "semaphore.h"

#include <core/log.h>
#include <thread>

namespace core {

  semaphore::semaphore()
    : count( 0 )
  {}

  void semaphore::notify() {
    std::unique_lock<mutex_type> lock(mutex);
    ++count;
    condition.notify_one();
  }

  void semaphore::wait() {
    std::unique_lock<mutex_type> lock(mutex);
    condition.wait( lock, [&]{ return bool(count); } );
    --count;
  }

  semaphore::semaphore(semaphore&& other) 
    : mutex(),
      condition(),
      count( other.count ) {

  }

  void semaphore::reset() {
    std::unique_lock<mutex_type> lock(mutex);
    count = 0;
  }



}
