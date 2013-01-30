#include "meuh.h"

#include <stdexcept>

namespace core {

  namespace meuh {

    std::string root() {
      try {
	return std::getenv ("MEUH_PATH");
      } catch( const std::logic_error& e) {
	throw std::logic_error("environment variable MEUH_PATH undefined !");
      }
    }
    
  }
}
