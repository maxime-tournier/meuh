#include <plugin/load.h>

#include <core/error.h>
#include <core/log.h>

#include <dlfcn.h>
#include <core/meuh.h>

#include <map>

namespace plugin {

  typedef std::map<std::string, ctor_type > added_type;
  static added_type added;

  static std::string so_name(const std::string& plugin_name) {
    const std::string ext = ".so";
    return "libplugin." + plugin_name + ext;
  }
  

  base* load(const std::string& name) {
    
    auto it = added.find( name ); 
    if( it != added.end() ) return it->second();
    
    const std::string libname = core::meuh::root() + "/lib/" + so_name(name);
    
    core::log("loading plugin:", name, libname);

    void *handle = dlopen(libname.c_str(), RTLD_NOW);
    if(handle == NULL){
      core::log( dlerror() );
    }
    
    // the dlopen should automagically register a constructor in the
    // map for the plugin through a static loader<> instance
    it = added.find( name );
    if( it == added.end() ) throw core::error("plugin error");
    
    return it->second();
  }
  
  void add(const std::string& name, const ctor_type& ctor) {
    added[ name ] = ctor;
  }

  void remove(const std::string& name) {
    added.erase( name );
  }
  
}
