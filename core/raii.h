#ifndef CORE_RAII_H
#define CORE_RAII_H

#include <core/callback.h>
#include <cassert>

namespace core {

  namespace raii {

    template<class A>
    struct starter {
      A& instance;
      
      starter(A& instance) : instance(instance) { instance.start(); }
      ~starter() { instance.stop(); }
    };

    template<class A>
    starter<A> start(A& instance) { return {instance}; }
    
    // TODO locker

    
    struct call {
      const callback f;
      call(const callback& f) : f(f) { assert(f); }
      ~call() { f(); }
    };
    
    
  }

}



#endif
