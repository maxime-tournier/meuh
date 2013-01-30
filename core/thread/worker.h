#ifndef CORE_THREAD_WORKER_H
#define CORE_THREAD_WORKER_H

#include <core/callback.h>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace core {
  namespace thread {

    class worker {

      std::mutex mutex;
      std::thread thread;
      std::condition_variable cond;
      core::callback action;
    public:
      
      struct stop_request : std::exception { };

      void start();
      void stop();
      
      void set(const core::callback& act);
      
      

    };

  }
}


#endif
