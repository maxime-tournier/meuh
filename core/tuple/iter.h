#ifndef CORE_TUPLE_ITER_H
#define CORE_TUPLE_ITER_H

#include <core/tuple/index.h>
#include <core/unused.h>
#include <core/noop.h>

namespace core {

  namespace tuple {
    
    namespace impl {
      
      
      template<class F, class ... Args, int ... I >
      void iter( const F& f, const std::tuple<Args...>& args, index<I...> ) {
	noop()( (f( std::get<I>(args)), 0)... );
      }

      template<class F, class ... Args, int ... I >
      void iter( const F& f, std::tuple<Args...>& args, index<I...> ) {
	noop()( (f( std::get<I>(args)), 0)... );
      }
      
      template<class F, class ... Args1, class ... Args2, int ... I >
      void iter2( const F& f, const std::tuple<Args1...>& args1, const std::tuple<Args2...>& args2, index<I...> ) {
	noop()( (f( std::get<I>(args1), std::get<I>(args2) ), 0)... );
      }
    }
    
    
    template<class F, class ... Args>
    void iter( const F& f, const std::tuple<Args...>& args) {
      impl::iter(f, args, make_index(args));
    }

    template<class F, class ... Args>
    void iter( const F& f, std::tuple<Args...>& args) {
      impl::iter(f, args, make_index(args));
    }

    template<class F, class ... Args1, class ... Args2>
    void iter2( const F& f, const std::tuple<Args1...>& args1, const std::tuple<Args2...>& args2) {
      static_assert( sizeof...(Args1) == sizeof...(Args2),
		     "tuples must have the same size" );
      impl::iter2(f, args1, args2, make_index(args1));
    }




  }

}


#endif
