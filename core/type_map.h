#ifndef CORE_TYPE_MAP_H
#define CORE_TYPE_MAP_H

#include <unordered_map>
#include <map>

#include <typeinfo>
#include <stdexcept>

#include <core/debug.h>

namespace core {


  // holds several values of different types
  class type_map {

    struct base { 
      virtual ~base();    
    };

    template<class U>
    struct object : base {
      U value;
    };

    typedef const std::type_info* key_type;
    typedef std::unordered_map<key_type, base* > impl_type;
    

    impl_type impl;
    
  public:
    type_map() = default;
    type_map(const type_map& ) = delete;
    
    ~type_map();

    template<class A>
    A& get( bool* inserted = 0 ) {
      auto key = &typeid(A);
      auto it = impl.find( key );

      if( it == impl.end() ) {
	base* obj = new object<A>;
	it = impl.insert(it, std::make_pair(key, obj) );
	if(inserted) *inserted = true;
	
      } else {
	if(inserted) *inserted = false;
	
      }
      
      return static_cast< object<A>* >(it->second)->value;
    }

    template<class A>
    const A& get() const {
      auto it = impl.find( &typeid(A) );

      if(it == impl.end()) {
	throw std::logic_error("type not found");
      }

      return static_cast< const object<A>* >(it->second)->value;
    }
    
    void clear();

  };

  
  



}


#endif
