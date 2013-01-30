#ifndef PLUGIN_NAME_H
#define PLUGIN_NAME_H

#include <string>
#include <meta/name.h>

namespace plugin {

  
  template<class Plugin>
  inline std::string name() {
    auto full = meta::name<Plugin>();
    auto pos = full.rfind("::");
    
    if( pos == std::string::npos ) return full;
    return full.substr(pos + 2, std::string::npos );
  }

}


#endif
