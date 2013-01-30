#include "resource.h"

#include <core/io.h>

#include <stdexcept>

namespace core {

  namespace resource {
    
    std::string dir() {
      try {
	return std::getenv ("MEUH_HOME");
      } catch( const std::logic_error& e) {
	throw std::logic_error("environment variable MEUH_HOME undefined !");
      }
    }
    
    std::string path(const std::string& relative) {
      return dir() + relative;
    }

    std::string ascii(const std::string& relative) {
      return core::io::load::ascii( path(relative) );
    }


  }
  
}
