#ifndef CORE_ERROR_H
#define CORE_ERROR_H

#include <core/exception.h>
#include <core/macro/here.h>
#include <stdexcept>

namespace core {

  struct error : public std::logic_error {
    error(const std::string& what);
  };

}


#endif

