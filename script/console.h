#ifndef SCRIPT_CONSOLE_H
#define SCRIPT_CONSOLE_H

#include <core/semaphore.h>

namespace script {

  // readline-based console
  class console {
    core::semaphore sema;
    
  public:
 
    void operator()() ;
    void exit();

  };

}

#endif
