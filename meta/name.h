#ifndef META_NAME_H
#define META_NAME_H

#include <typeinfo>
#include <string>
#include <cxxabi.h>

namespace meta {
  
  template<class T>
  std::string name() { 
    // see http://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
    char* realname;
    int status;
    realname = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
    std::string res(realname);
    free(realname);
    return res;
  }

}



#endif
