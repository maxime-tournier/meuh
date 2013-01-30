#ifndef CORE_SHARED_H
#define CORE_SHARED_H


namespace core {


  template<class U>
  class shared {
    U value;
    
    typedef std::recursive_mutex mutex_type;
    mutable mutex_type mutex;
  public:

    template<class ... Args>
    shared(Args&& ... args) : value( std::forward<Args>(args)... ) { }
    

    template<class T>
    struct getter {
      
      T& ref;

      mutex_type& mutex;
      
      getter(T& ref, mutex_type& mutex) 
	: ref(ref),
	  mutex(mutex) {
	mutex.lock();
      }

      ~getter() {
	mutex.unlock();
      }
      
      T* operator->() const volatile{
	return &ref;
      }

      T& operator*() const volatile{
	return ref;
      }
      

    };
    
    
    getter<U> lock()  {
      return {value, mutex};
    }

    getter<const U> lock() const {
      return {value, mutex};
    }
      
  };


}


#endif
