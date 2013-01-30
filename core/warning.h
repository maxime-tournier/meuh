#ifndef CORE_WARNING_H
#define CORE_WARNING_H

#include <core/log.h>

namespace core {


  template<class ... Args>
  void warning(Args&& ... args) {
    core::log("warning:", std::forward<Args>(args)...);
  }

}


#endif
