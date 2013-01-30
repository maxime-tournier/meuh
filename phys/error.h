#ifndef PHYS_ERROR_H
#define PHYS_ERROR_H

#include <core/error.h>
#include <phys/exception.h>

namespace phys {
  
  struct error : core::error {
    error(const std::string& what);
  };


}



#endif
