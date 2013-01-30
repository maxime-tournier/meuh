

#include <script/lua.h>
#include <lua5.1/lua.hpp>
#include <luabind/luabind.hpp>

#include <iostream>
#include <script/ref.h>

#include <core/meuh.h>
#include <core/log.h>
#include <core/type.h>

#include <luabind/adopt_policy.hpp>
#include <luabind/copy_policy.hpp>

namespace script {
  
  template<class FuncType>
  static luabind::class_< std::function<FuncType> > func() {
    // we use g++ name mangling to name types
    typedef std::function<FuncType> func_type;
    return 
      luabind::class_< func_type >( typeid(FuncType).name() )
      .def("__call", &func_type::operator());
  }

  template<class Value>
  static luabind::class_< reference<Value> > var() {
    luabind::class_< reference<Value> > res( typeid(Value).name() );
    
    res.def("get", &reference<Value>::get );
    res.def("set", &reference<Value>::set);
    
    return res;
  }

  template<class Value>
  static luabind::class_< reference<const Value> > const_var() {
    luabind::class_< reference<const Value> > res( typeid(const Value).name() );
    res.def("get", &reference<const Value>::get );
    return res;
  }


}


// this is where we expose c++ stuff to lua
extern "C" int init(lua_State* L)
{
    using namespace luabind;
    using namespace script;
    open(L);

    module(L)
      [
       // wrapped base types
       var<double>(),
       const_var<double>(),
       var<int>(),
       var<unsigned int>(),
       var<bool>(),
       
       // binding functions
       func< void ( ) >(),
       func< void ( bool ) >(),
       func< void ( int ) >(),
       func< void ( unsigned int ) >(),
       func< void ( double ) >(),
       func< void ( const std::string& ) >(),
       func< void ( const char* ) >(),
       func< void ( unsigned int, unsigned int ) >(),
       func< void ( unsigned int, double ) >(),
       func< void ( int, double ) >(),
       func< void ( double, double, double ) >(),
       func< void ( unsigned int, bool ) >(),
       func< void ( const std::string&, double ) >(),
       func< void ( const std::string&, double, double ) >(),
       func< void ( const std::string&, double, double, double ) >(),
       func< void ( const std::string&, unsigned int ) >(),
       func< std::string () >(),
       func< std::string (const std::string&) >(),
       func< unsigned int () >(),
       func< double () >(),
       func< bool () >(),
       func< unsigned int ( const std::string& ) >(),
       func< reference<double> () >(),
       func< reference<double> (unsigned int) >(),
       func< reference<const double> () >(),
       func< reference<const double> (unsigned int) >()
       ];
     
    return 0;
}


