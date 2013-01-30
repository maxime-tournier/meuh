#ifndef MATH_ERROR_H
#define MATH_ERROR_H

#include <core/error.h>

namespace math {

  struct error : core::error {
    error(const std::string& what);
  };


}

#endif
