#ifndef CORE_LOCAL_H
#define CORE_LOCAL_H

#include <thread>
#include <map>
#include <memory>
#include <mutex>

namespace core {


  template<class U>
  class local {
    std::map< std::thread::id, U> data;
    std::recursive_mutex mutex;
  public:

   
    
    local(const local& other) 
      : data(other.data) {

    }

    local(local& other) 
      : data(other.data) {

    }

    local(local&& other) 
      : data(std::move(other.data) ) {
      
    }

    local() { }

    template<class ... Args>
    local(Args&& ... args) {
      data.insert(std::make_pair(std::this_thread::get_id(),
				 U(std::forward<Args>(args)... )) );
    }

    U& operator*() { 
      std::unique_lock< std::recursive_mutex > lock(mutex);

      auto id = std::this_thread::get_id();
      auto it = data.find( id );
      
      if(it == data.end() ) {
	it = data.insert(data.end(), std::make_pair(id, data.begin()->second ) );
      }
      
      return it->second;
    }
    
    U* operator->() {
      return &operator*();
    }
    
  };



}


#endif
