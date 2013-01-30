#ifndef MATH_FUNC_INHERIT_H
#define MATH_FUNC_INHERIT_H

#include <utility>

// TODO move to core ?

namespace math {
  namespace func {

    template<class F>
    struct base : F {
      typedef F type;
      
      template<class ... Args>
      base(Args&& ... args) : type{ std::forward<Args>(args)... } { }
      
      base(const base& ) = default;
      base(base&&) = default;


    };

  }
  
}


#endif
