#ifndef TUPLE_TAIL_H
#define TUPLE_TAIL_H

#include <core/tuple/apply.h>

namespace core {
  namespace tuple {

      
    template<class > struct tail;

    template<class A, class ... Args > 
    struct tail< std::tuple<A, Args... > > {
      typedef std::tuple< Args... > type;
    };

      
      
    struct pack_tail {
      template<class A, class ... Args>
      std::tuple<Args...> operator()(const A&, const Args&... args) const {
	return std::make_tuple(args...);
      }
    };

    
    

    template<class A, class ... Args>
    std::tuple<Args...> get_tail(const std::tuple<A, Args...>& args) {
      return apply( pack_tail(), args );
    }
    

  }

}


#endif
