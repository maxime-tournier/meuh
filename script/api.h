#ifndef SCRIPT_API_H
#define SCRIPT_API_H

#include <script/lua.h>
#include <script/ref.h>
#include <luabind/luabind.hpp>

#include <map>

#include <core/func.h>

// TODO this should be called module !

namespace script {
  
  // aggregates C++ values/functions to be exposed in a lua table
  class api {
    typedef std::function<void (luabind::object& ) > callback_type;
    std::map<std::string, callback_type > callbacks;

    void fill(luabind::object& obj) const;
    
    const std::string name;
    void setup();

  public:
    
    api(const std::string& name);
    api(const api& ) = delete;
    api(api&& );
    
    template<class Value>
    api& ref(const std::string& name,
	     Value& value) {
      
      callbacks[ name ] = [name, &value](luabind::object& obj) {
	luabind::settable(obj, name, script::ref(value) );
      };
      
      return *this;
    }

    template<class F>
    api& fun(const std::string& name,
	     const F& f) {
      
      // we hide f under an std::function to register it with luabind
      std::function< typename core::func<F>::type > fun = f;

      callbacks[name] = [name, fun](luabind::object& obj) {
      	luabind::settable(obj, name, fun );
      };
      
      return *this;
    }
    
    bool remove(const std::string& name);

  };
  
  
}


#endif
