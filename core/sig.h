#ifndef CORE_SIG_H
#define CORE_SIG_H

#include <core/callback.h>

namespace core {
  namespace sig {
    
    callback& handler(int signo);
    void clear(int signo );
    
  }
}

#endif
