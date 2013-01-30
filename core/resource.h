#ifndef CORE_RESOURCE_H
#define CORE_RESOURCE_H

#include <string>

namespace core {

  namespace resource {
    
    // base dir
    std::string dir();

    
    std::string path(const std::string& relative);

    // loads ascii file
    std::string ascii(const std::string& relative);

  }
}



#endif
