#ifndef CORE_MIXIN_H
#define CORE_MIXIN_H

namespace core {
  

  template<class ... Args>
  struct mixin : Args... { 
    
    mixin(const Args&... args) : Args(args)... { }

  };


  template<class ... Args>
  mixin<Args...> mix(const Args&... args) { return mixin<Args...>(args...); }

}

#endif
