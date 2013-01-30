#include "worker.h"

namespace core {
  namespace thread {


    void worker::set(const core::callback& act) {

      auto body = [&] {
	action = act;
      };

      // we don't lock if called from worker thread
      if( thread.get_id() != std::this_thread::get_id() ) {
	std::unique_lock<std::mutex> lock(mutex);
	body();
	cond.notify_one();
      } else {
	body();
      }
    }
    

    void worker::start() {

      thread = std::thread([&] {
	  std::unique_lock<std::mutex> lock(mutex);

	  try {
	    while(true) {

	      if( !action ) cond.wait(lock, [&]{ return action; });
	      auto copy = action;
	      action = 0;
	      copy();

	    };
	  }
	  catch( stop_request ) {
	    
	  }
	    
	});
    }

    void worker::stop() {
      set( [] { throw stop_request(); } );
    }
    
  }
}
