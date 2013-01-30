#ifndef MATH_FUNC_VECTORIZED_H
#define MATH_FUNC_VECTORIZED_H

#include <math/func.h>
#include <math/func/ref.h>
#include <math/euclid.h>
#include <math/vec.h>

namespace math {
  namespace func {

    // for euclidean spaces, use this to adapt mappings to iterative
    // solvers
    template<class F>
    struct vectorized {

      typedef math::vec domain;
      typedef math::vec range;

      F of;

      typedef typename traits<F>::domain of_domain;
      typedef typename traits<F>::range of_range;

      typename math::euclid::space<of_domain> dmn;
      typename math::euclid::space<of_range> rng;
      
      mutable of_domain arg;
      mutable range res;
      
      vectorized(const F& of,
		 const math::euclid::space<of_domain>& dmn,
		 const math::euclid::space<of_range>& rng)
	: of(of),
	  dmn(dmn),
	  rng(rng),
	  arg( dmn.zero() ),
	  res( vec::Zero( rng.dim() ) ) {
	
      }
      
      const range& operator()(const domain& x) const {
	dmn.set(arg, x);
	rng.get(res, of( arg ));
	return res;
      };
      

      struct push : base< vectorized< typename traits<F>::push > > {
	push(const vectorized& of, const domain& at)
	  : push::base(d( ref(of) )(at), of.dmn, of.rng) {

	}
      };

      struct pull : base< vectorized< typename traits<F>::pull > > {
	pull(const vectorized& of, const domain& at)
	  : pull::base(dT( ref(of) )(at), of.dmn, of.rng) {

	}
      };


    };

    template<class F>
    vectorized<F> vectorize(const F& of, 
			    const math::euclid::space< typename traits<F>::domain>& dmn = 
			    math::euclid::space<typename traits<F>::domain>(),
			    
			    const math::euclid::space<typename traits<F>::range>& rng = 
			    math::euclid::space<typename traits<F>::range>() ) {
      return {of, dmn, rng};
    }
			    


  }
}



#endif
