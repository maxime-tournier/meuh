#ifndef CORE_BIND_H
#define CORE_BIND_H

#include <core/tuple/index.h>
#include <utility>

namespace core {
  

  template<class Fun, class ... Fixed>
  struct binding {

    const Fun fun;
    const std::tuple<Fixed&&...> fixed; // TODO does this store rvalues ?
    
    binding(const Fun& fun, Fixed&& ... fixed) 
      : fun(fun),
	fixed( std::forward<Fixed>(fixed)... ) {
      
    }

    
    template<int ... I, class ... Args>
    typename std::result_of< Fun(Fixed&&..., Args&&...) >::type
    impl(tuple::index<I...>, Args&& ... args) const {
      return fun( std::get<I>(fixed)... , std::forward<Args>(args)...);
    }

    template<class ... Args>
    typename std::result_of< Fun(Fixed&&..., Args&&...) >::type 
    operator()(Args&& ... args) const  {
      return impl(tuple::make_index(fixed), std::forward<Args>(args)...);
    }
    
  };
  
    

  template<class Fun>
  struct binder {

    const Fun fun;

    template<class ... Fixed>
    binding<Fun, Fixed...> operator()(Fixed&& ... fixed) const {
      return binding<Fun, Fixed...>{fun, std::forward<Fixed>(fixed)...};
    }
    
  };

  template<class Fun>
  binder<Fun> bind(const Fun& fun) { 
    return {fun};
  }
  
}



#endif
