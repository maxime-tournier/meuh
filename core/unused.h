#ifndef CORE_UNUSED_H
#define CORE_UNUSED_H

// keep unused variables noise low

namespace core {
  
  struct unused {
    template<class ... A> unused(A&& ... ) { }
    
  };
  

  
}



#endif
