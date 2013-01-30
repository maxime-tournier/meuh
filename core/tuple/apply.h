#ifndef CORE_TUPLE_APPLY_H
#define CORE_TUPLE_APPLY_H

#include <core/tuple/index.h>
#include <core/make.h>

namespace core {
  namespace tuple {

    template<class F, class ... Args, int ... I>
    typename std::result_of<F(const Args&...)>::type
    apply(const F&f, const std::tuple<Args...>& args, index<I...> ) 
    {
      return f( std::get<I>(args)...);
    };
    
    template<class F, class ... Args, int ... I>
    typename std::result_of<F(Args&...)>::type
    apply(const F&f, std::tuple<Args...>& args, index<I...> ) 
    {
      return f( std::get<I>(args)...);
    };



    template<class F, class ... Args>
    typename std::result_of<F(const Args&...)>::type
    apply(const F&f, const std::tuple<Args...>& args) 
    {
      return apply(f, args, make_index(args) );
    };

    template<class F, class ... Args>
    typename std::result_of<F(Args&...)>::type
    apply(const F&f, std::tuple<Args...>& args) 
    {
      return apply(f, args, make_index(args) );
    };


  }
}

#endif
