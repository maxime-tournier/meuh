#ifndef MOCAP_ERROR_H
#define MOCAP_ERROR_H

#include <core/error.h>

namespace mocap {
  struct error : core::error {  
    error( const std::string& what ) : core::error(what) { }
  };

}

// TODO cpp

#endif
