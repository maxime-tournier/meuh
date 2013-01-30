#include "thread.h"

#ifdef WIN32 

namespace std {
  
  namespace this_thread {
    
    thread::id get_id() { return 0; }

  }

}


#endif
