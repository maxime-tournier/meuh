#include "lua.h"

#include <core/log.h>
#include <cassert>

namespace script {

  lua::lua() : _state( lua_open() ) { 
    assert( state() );
  }
  
  lua::~lua() { 
    lua_close(state()); 
  }

  lua_State* lua::state() const { return _state; }
  
  
  int lua::string(const std::string& s) const {
    return luaL_dostring( state(), s.c_str() );
  }

  int lua::file(const std::string& filename) const {
    return luaL_dofile( state(), filename.c_str() );
  }


  std::string lua::at(int pos) const {
    return lua_tostring( state(), pos);
  }


  std::string lua::pop( unsigned int i ) const {
    const std::string res = at( -int(i) ); 
    lua_pop(state(), i);

    return res;
  }


  void lua::libs() const{
    luaL_openlibs( state() );
  }
  

  // calls the pushed callback
  static int closure_call(lua_State* L) {
    void* ptr = lua_touserdata(L, lua_upvalueindex(1));
    return static_cast< lua::function_type* >(ptr)->operator()(L);
  };


  void lua::closure(const function_type& fun) {
    // TODO thread safety ?
    
    // we store and manage a copy
    storage.push_back(fun);
    function_type* ptr = &storage.back();
    
    // push callback address
    lua_pushlightuserdata(state(), ptr);

    // push caller function 
    lua_pushcclosure(state(), &closure_call, 1);
  }

  void lua::global(const std::string& name) const {
    lua_setglobal(state(), name.c_str());
  }

  // void lua::get_global(const std::string& name) {
  //   lua_getglobal(state(), name.c_str());
  // }

  // void lua::set_field(int index, const std::string& name) {
  //   lua_getfield(state(), index, name.c_str() );
  // }


  int lua::exec(const std::string& cmd) const {
    int error = string( cmd );
    if( error ) { 
      core::log( pop() );
    }
    
    return error;
  }
  
}
