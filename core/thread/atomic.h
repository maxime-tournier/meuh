#ifndef CORE_THREAD_ATOMIC_H
#define CORE_THREAD_ATOMIC_H

#include <mutex>
#include <utility>

namespace core {
  namespace thread {

    template<class Data>
    class atomic {
      Data data;
      typedef std::recursive_mutex mutex_type;
      mutable mutex_type mutex;
      typedef std::unique_lock< mutex_type > lock_type;
    public:
      
      template<class ... Args>
      atomic(Args&& ... args) 
	: data(std::forward<Args>(args)...) { } 

      
      template<class T>
      class handle {
	T* ptr;
	lock_type lock;
      public:
	handle(T* ptr, lock_type&& lock) 
	  : ptr(ptr),
	    lock( std::move(lock) ) {

	}
	
	handle(handle&&) = default;
	handle(const handle& ) = delete;

	handle& operator=(const handle& ) = delete;
	handle& operator=(handle&& ) = default;
	
	T* operator->() const { return ptr; }
	T* get() const { return ptr; }
	T& operator*() const { return *ptr; }
	
      };
      
      handle<Data> operator()() { return {&data, lock_type(mutex) }; }
      handle<const Data> operator()() const { return {&data, lock_type(mutex) }; }
      
    };
    


  }
}



#endif
