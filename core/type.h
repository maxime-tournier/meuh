#ifndef CORE_TYPE_H
#define CORE_TYPE_H

#include <typeinfo>
#include <string>

#include <cxxabi.h>

namespace core {

  // this is a general type trait
  template<class T>
  struct type { 
    // used to pretty-print types
    static std::string name() { 
      // see http://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
      char* realname;
      int status;
      realname = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
      std::string res(realname);
      free(realname);
      return res;
    }
  };

  // namespace impl {
  //   template<class H1, class H2, class ... Args>
  //   std::string args_name() { return type<H1>::name() + ", " + args_name<H2, Args...>() ; }
  
  //   template<class Head>
  //   std::string args_name() { return type<Head>::name() + ">"; }
  // }
  
  // template<class ... Args>
  // std::string args_name() { return std::string("<") + impl::args_name<Args...>(); }
  
}

#endif
