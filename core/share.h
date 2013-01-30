#ifndef CORE_SHARE_H
#define CORE_SHARE_H

#include <string>

namespace core {

  // get shared data (in meuh/share directory)
  std::string share(const std::string& relative = std::string() );
  
}


#endif
