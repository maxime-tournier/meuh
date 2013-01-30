#ifndef SCRIPT_CALL_H
#define SCRIPT_CALL_H

#include <script/lua.h>
#include <luabind/luabind.hpp>

namespace script {

  
  template<class Ret, class ... Args>
  Ret call(script::lua& lua, const std::string& name,
	   Args&& ... args) {
    
    return luabind::call_function<Ret>(lua.state(), name.c_str(),
				       std::forward<Args>(args)... );
    
  }


}



#endif
