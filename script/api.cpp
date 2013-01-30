#include "api.h"

#include <core/stl.h>
#include <core/string.h>

#include <script/meuh.h>

namespace script {
  
  // fills the result table with lua members
  void api::fill(luabind::object& obj) const {
    core::each( callbacks, [&](const std::string&, const callback_type& fun) {
	fun(obj);
      });
  }

  bool api::remove(const std::string& name) {
    auto it = callbacks.find(name);

    if( it == callbacks.end() ) return false;
    callbacks.erase(it);

    return true;    
  }
  
  
  // creates lua closure, and register it under the global name api.@name()
  void api::setup() {
    // pushes the following closure
    the()->closure([&](lua_State* L) -> int {
	
	luabind::object obj = luabind::newtable( L );
	this->fill( obj );
	obj.push(L);
	return 1;
	
      });
    
    // we push the above closure as a global with the following name
    const std::string id = "__derp";
    the()->global( id );
    
    // we retrieve the global and register it, then free the global
    // name
    script::exec("api.add('%%', %%); %% = nil", name, id, id);

  }

  api::api(const std::string& name) 
  : name(name) {
    require("api");
    setup();
  }
  
  api::api(api&& other) 
    : callbacks( other.callbacks ),
      name( other.name )
  {
    setup();
  }
  
}
