#ifndef GEO_ERROR_H
#define GEO_ERROR_H

#include <core/error.h>

namespace geo {
  

  struct error : core::error {
    error(const std::string& what);
  };


  // TOOD CPP ?
}




#endif
