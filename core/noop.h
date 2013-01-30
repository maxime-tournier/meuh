#ifndef CORE_NOOP_H
#define CORE_NOOP_H

namespace core {
  
  // does nothing :)
  struct noop {

    template<class ... Args>
    void operator()(Args&& ... ) const { }

  };

}


#endif
