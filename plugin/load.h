#ifndef PLUGIN_LOAD_H
#define PLUGIN_LOAD_H

#include <string>
#include <memory>

#include <plugin/name.h>

namespace plugin {

  struct base;

  // TODO unload ?
  base* load(const std::string& name);

  template<class Plugin>
  inline std::unique_ptr<Plugin> load() { 
    return std::unique_ptr<Plugin>( static_cast<Plugin*>( load( name<Plugin>()) ) );
  }
  
  
  // TODO forward arguments to ctor ?
  typedef std::function< base* () > ctor_type;
  
  void add(const std::string& name, const ctor_type& ctor);
  void remove(const std::string& name);
  
  // plugins must declare a static loader somewhere in the library
  template<class Plugin>
  struct loader {
      
    loader( std::function< Plugin* () > ctor )  {
      add( name<Plugin>(), ctor );
    }
  
    ~loader() {
      remove( name<Plugin>() );
    }
    
  };


}


#endif
