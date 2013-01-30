#ifndef CORE_TUPLE_ZIP_H
#define CORE_TUPLE_ZIP_H

#include <core/tuple/index.h>
#include <core/make.h>

namespace core {
  namespace tuple {  

    // zips two tuples with a function
    namespace impl {
      

      template<class F, class ... Args1, class ... Args2, int ... I>
      std::tuple< typename std::result_of<F(Args1, Args2)>::type... >
      zip(const F& f, 
	  const std::tuple<Args1...>& a, 
	  const std::tuple<Args2...>& b,
	  tuple::index<I...> ) 
      {
	static_assert( (sizeof...(Args1) == sizeof...(Args2)) && 
		       (sizeof...(Args2) == sizeof...(I) ),
		       "tuple size mismatch" );
	return std::make_tuple( f(std::get<I>(a), std::get<I>(b))... );
      } 

    }
    
    
    template<class F, class ... Args1, class ... Args2>
    std::tuple< typename std::result_of<F(Args1, Args2)>::type... >
    zip(const F& f, const std::tuple<Args1...>& a, const std::tuple<Args2...>& b ) {
      return impl::zip(f, a, b, make_index(a) );
    } 
    


  }
}

#endif
