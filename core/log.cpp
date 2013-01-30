#include "log.h"

namespace core {
  // log::log(std::ostream& stream) : stream(stream) { }
 
  // std::ostream& log::operator()() const { return stream << std::endl; }


  logger::logger(std::ostream& stream) : enabled(true), stream(stream) { }
  
  std::ostream& logger::operator()() const { 
    if(!enabled) return stream;
    return stream << std::endl; 
  }

  static logger instance;
  logger& log() { return instance; }

}
