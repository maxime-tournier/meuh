#ifndef CORE_THREAD_CONSUMER_H
#define CORE_THREAD_CONSUMER_H

#include <core/thread/queue.h>
#include <core/thread/atomic.h>

#include <core/log.h>
#include <core/exception.h>
#include <core/thread/worker.h>

namespace core {
  namespace thread {

    template<class Task>
    class consumer {
      std::thread thr;
      atomic<bool> started;

      void loop() {

	auto lock = queue.lock();

	try {
	  while(true) { 
	    
	    queue.wait(lock);
	    while(!queue.empty()) {
	      action( queue.pop() );
	    }
	    
	  }
	} 
	catch( const exit& ) { }
	 
      }

    public:
      struct exit : core::exception { };
      
      thread::queue<Task>& queue;

      typedef std::function< void (Task&& )> action_type;
      const action_type action;
      
      consumer(thread::queue<Task>& queue,
	       const action_type& action) 
	: queue(queue),
	  action(action), 
	  started(false) { }
      
      void start() {
	thr = std::thread( [&] { 
	    // TODO raii ?
	    started().get() = true;
	    this->loop();  
	    started().get() = false;
	  });
      }
      
      
      void wait() {
	thr.join();
      }

      bool running() const { return started().get(); }
      

      const std::thread& thread() const { return thr; }
    };


  }
}



#endif
