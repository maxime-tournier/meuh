#include "worker.h"

namespace core {



  void worker::operator()() {
    while(true) {
      std::unique_lock<std::mutex> lock(mutex);
      cond.wait(lock,[&] () {return bool(task);});
      task();
      task.reset();
    }
  }
  
  
  std::future<void> worker::give(const std::function< void() >& f) {
    std::lock_guard<std::mutex> lock(mutex);
    task = std::packaged_task<void()>(f);
    cond.notify_one();
    
    return task.get_future();
  }


}
