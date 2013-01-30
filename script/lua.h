#ifndef SCRIPT_LUA_H
#define SCRIPT_LUA_H

#include <lua5.1/lua.hpp>
#include <string> 
#include <functional>
#include <list>

namespace script {

  class lua {
  public:

    // TODO remove header dependency on lua ?
    typedef lua_State* state_type;
    typedef std::function< int (state_type) > function_type;

  private:
    state_type _state;
    std::list< function_type > storage;
    
  public:
    lua();
    ~lua();
    
    state_type state() const;
    
    // exec string/file
    int string(const std::string& s) const;
    int file(const std::string& filename) const;
    
    // convenience: exec and display error if any
    int exec(const std::string& cmd) const ;

    // stack management
    std::string at(int pos) const;
    std::string pop( unsigned int i = 1) const;
    
    // open libs
    void libs() const;
    
    // pushes a std::function onto the stack as a c closure 
    // pushes 2 elements !
    void closure(const function_type& );
    
    // gives the element onto the stack a global name (set_global)
    void global(const std::string& name) const;
    
    // // pushes onto the stack the element with global name
    // void get_global(const std::string& name) const;
    
    
    // void set_field(int index, const std::string& name) const;
    // void get_field(int index, const std::string& name) const;
    
  };


}


#endif
