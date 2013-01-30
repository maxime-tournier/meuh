#ifndef MATH_PUSH_H
#define MATH_PUSH_H

#include <math/func.h>

#include <meta/name.h>
#include <meta/val.h>

#warning obselete !

namespace math {

  template<class F, class = meta::enable>
  struct Push;

  // when member typenames are defined
  template<class F>
  struct Push<F, typename meta::sfinae< typename F::push >::type > {
    typedef typename F::push type;
  };

  // // fallback
  // template<class F, class>
  // struct Push {
  //   struct type {
    
  //     type(const F& of, const typename Func<F>::domain& at) { }
    
  //     typename Lie< typename Func<F>::range >::algebra 
  //     operator()(const typename Lie<typename Func<F>::domain >::algebra& ) const {
  // 	throw error(meta::name<F>() + " has no pushforward");
  //     }
  //   };
  // };




    
  template<class F>
  void fallback_pushforward( ) {
    bool trigger;
  }

  
  // fallback: old-style interface adaptor
  template<class F, class>
  struct Push {
    
    struct type {
      
      const F of;
      const typename Func<F>::domain at;

      typename Lie< typename Func<F>::range >::algebra 
      operator()(const typename Lie<typename Func<F>::domain >::algebra& body) const {
	fallback_pushforward<F>();
	return math::push(of, math::body(at, body) ).body();
      }
      
    };
    
  };
}


namespace meta {
  
  template<class F>
  struct Concept< math::Push<F> > {
  
    ~Concept() {
      using namespace math;

      typedef typename Push<F>::type type;

      concept_assert<Func<type> >();
      
      concept_assert< Equal< typename Func< type >::domain, typename Lie< typename Func<F>::domain >::algebra > >();
      
      concept_assert< Equal< typename Func< type >::range,  typename Lie< typename Func<F>::range>::algebra>  >();
      
      // construction requirements
      const type push{val< F >(), val<  typename Func<F>::domain > () };
    }  
    
 
    
  };
}



#endif
