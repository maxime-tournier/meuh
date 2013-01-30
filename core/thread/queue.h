#ifndef CORE_THREAD_QUEUE_H
#define CORE_THREAD_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

namespace core {
  namespace thread {

    template<typename Data>
    class queue {
    private:
      typedef std::mutex mutex_type;
      typedef std::unique_lock<mutex_type> lock_type;
      
      typedef std::queue<Data> queue_type;
      queue_type queue;
      mutable mutex_type mutex;

      std::condition_variable cond;
    public:
      
      void push(Data&& data) {
        lock_type lock(mutex);
        queue.push( std::move(data) );
        lock.unlock();
        cond.notify_one();
      }

      bool empty() const {
        lock_type lock(mutex);
        return queue.empty();
      }
      
      template<class Action>
      typename std::result_of< Action(Data&&) >::type wait(const Action& action) {
        lock_type lock(mutex);
        while( queue.empty() ) {
	  cond.wait(lock);
	}
        
	Data front = std::move( queue.front() );
	queue.pop();

	lock.unlock();
	return action( std::move(front) );
      }

    };

    
  }
}



#endif
