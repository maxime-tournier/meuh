#ifndef CORE_SEMAPHORE_H
#define CORE_SEMAPHORE_H

#include <mutex>
#include <condition_variable>
#include <core/uint.h>

namespace core {

  class semaphore {

    typedef std::mutex mutex_type;
    mutex_type mutex;
    std::condition_variable condition;
    volatile uint count;
    
  public:
    semaphore();

    semaphore(const semaphore& ) = delete;
    semaphore(semaphore&& );

    void notify();
    void wait();
    void reset();
    
  };


}


#endif
