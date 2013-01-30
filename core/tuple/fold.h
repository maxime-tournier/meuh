#ifndef CORE_TUPLE_FOLD_H
#define CORE_TUPLE_FOLD_H

#include <core/tuple/apply.h>
#include <core/make.h>


namespace core {
  namespace tuple {
    
    template<class F, class Init>
    struct lfolder {

      const F& with;
      const Init& init;		// ref ?

      template<class A, class ... B>
      Init operator()(A&& a, B&&... b) const;
      
      Init operator()() const  { return init; }

    };

    template<class F, class Init>
    lfolder<F, Init> make_lfolder(const F& with, const Init& init ) { return {with, init}; }
   
    
    template<class F, class Init>
    template<class A, class ... B>
    Init lfolder<F, Init>::operator()(A&& a, B&&... b) const {
      return make_lfolder( with, with(init, std::forward<A>(a)))( std::forward<B>(b)... );
    }
    

    template<class F, class Init, class ... Args>
    Init foldl(const F& f, const Init& init, const std::tuple<Args... >& args ) 
    {
      return apply( make_lfolder( f, init ), args );
    }

    template<class F, class Init, class ... Args>
    Init foldl(const F& f, const Init& init, std::tuple<Args... >& args ) 
    {
      return apply( make_lfolder( f, init ), args );
    }

  }
}

#endif
