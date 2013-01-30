#ifndef CORE_TUPLE_MAP_H
#define CORE_TUPLE_MAP_H

#include <core/tuple/apply.h>
#include <core/make.h>

#include <utility>
#include <core/log.h>
#include <core/type.h>

namespace core {
  namespace tuple {

    namespace impl {

      template<class F, class ... Args>
      struct map {
      	const F& f;
	
	typedef std::tuple< typename std::result_of< F(Args) >::type... > type;
	
      	type operator()(const Args&... args) const {
	  // core::log("tuple map result", core::type<type>::name());
      	  return type( f( args )... );
      	}
	
      };
      
      
      
    }


    template<class F, class ... Args>
    typename impl::map<F, Args...>::type
    map(const F& f, const std::tuple<Args...>& args) 
    {
      return apply( impl::map<F, Args...>{f}, args);
    } 

  }
}

#endif
