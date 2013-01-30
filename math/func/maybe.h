#ifndef MATH_FUNC_MAYBE_H
#define MATH_FUNC_MAYBE_H

#include <math/func.h>

// TODO improve ?

// TODO rename conditional/cond

namespace math {
  namespace func {

    template<class Pred, class F1, class F2>
    struct maybe {
      Pred pred;
      
      F1 f1;
      F2 f2;
      

      maybe(const Pred& pred, const F1& f1, const F2& f2) 
	: pred(pred),
	  f1(f1),
	  f2(f2) {

      }

      static_assert( std::is_same< typename traits<F1>::domain, 
				   typename traits<F2>::domain >::value, "durr");
    
      static_assert( std::is_same< typename traits<F1>::range,
				   typename traits<F2>::range >::value, "durr");
  
      typedef typename traits<F1>::domain domain;
      typedef typename traits<F1>::range range;

      // TODO optimized return types ?
      range operator()(const domain& x) const {
	if( pred() ) {
	  return f1(x);
	} else {
	  return f2(x);
	}
      }


      struct push : base< maybe<Pred, typename traits<F1>::push, typename traits<F2>::push > >{
	
	// TODO refs ?
	push(const maybe& of, const domain& at)
	  : push::base(of.pred, d(of.f1)(at), d(of.f2)(at)) {

	}
	
      };

      struct pull : base< maybe<Pred, typename traits<F1>::pull, typename traits<F2>::pull >  >{
	
	// TODO refs ?
	pull(const maybe& of, const domain& at)
	  : pull::base(of.pred, dT(of.f1)(at), dT(of.f2)(at)) {
	   
	}
	
      };



    };


    template<class Pred, class F1, class F2>
    maybe<Pred, F1, F2> make_maybe(const Pred& pred, const F1& f1, const F2& f2) {
      return {pred, f1, f2};
    }
    
  }
}


#endif
