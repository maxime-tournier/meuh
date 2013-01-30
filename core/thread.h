#ifndef CORE_THREAD_H
#define CORE_THREAD_H

#include <map>
#include <mutex>

#ifndef WIN32
#include <thread>
#else

namespace std {

  namespace thread {
    typedef unsigned int id;
  };

  namespace this_thread {
    thread::id get_id();
  }

  struct recursive_mutex { };
  template<class C> struct unique_lock {  unique_lock(C&) { } };

}

#endif




namespace core {

  namespace thread {

    

    template<class V>
    class local {
      mutable std::map<std::thread::id, V> data;
      mutable std::recursive_mutex mutex;

      V& at( std::thread::id id ) const {
	std::unique_lock< std::recursive_mutex > lock(mutex);
	return data[ id ];
      }
     
    public:

      
      local(){}
      local(const local& other ) : data(other.data) {}
      
      // TODO FIX mutex ownership ?
      local(local&& other) : data( std::move(other.data) ) {}

      V& get() const {
	return at( std::this_thread::get_id() );
      }
      
    };


  }

}



#endif
