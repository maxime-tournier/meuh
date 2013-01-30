#ifndef MATH_FUNC_NUMERICAL_H
#define MATH_FUNC_NUMERICAL_H

// TODO rename num.h ?

#include <math/types.h>
#include <math/func.h>

#include <math/func/lie/translation.h>
#include <math/func/ref.h>


namespace math {
  namespace func {

    
    template<class F>
    struct numerical {

      F of;
      math::real eps;
      
      // typedef typename traits<F>::domain domain;
      // typedef typgename traits<F>::range range;

      // HACK FIXME this is for numerical differentiation in
      // functional derivative: traits<F>::push can't be computed (see
      // apply.h)
      typedef typename traits<F>::domain domain;
      typedef typename traits<F>::range range;
      
      
      range operator()(const domain& x) const { return of(x); }
      
      
      struct push {

	F of;

	domain at;
	range end;
	
	const math::lie::group<domain> dmn;
	const math::lie::group<range> rng;
	
	const real eps;

	push(const numerical& of, const domain& at) 
	  : of(of.of),
	    at(at),
	    end(of(at)),
	    dmn( math::lie::of(at)),
	    rng( math::lie::of(end)),
	    eps(of.eps),
	    log(rng.log()),
	    L_end_inv( rng.inv(end) ),
	    delta(dmn.alg().zero()),
	    exp(dmn.id())
	{ 

	}

	typename math::lie::group<range>::logarithm log;
	lie::left<range> L_end_inv;

	mutable typename math::lie::group<domain>::algebra delta;
	mutable domain exp;
	
	
	typename math::lie::group<range>::algebra operator()
	(const typename math::lie::group<domain>::algebra& v) const {
	  delta = dmn.alg().scal(eps, v);
	  exp = dmn.exp()( delta );
	  
	  auto forward  = ref(L_end_inv) << ref(of) << lie::R( exp );
	  auto backward = ref(L_end_inv) << ref(of) << lie::R( dmn.inv(exp) );
	  
	  auto diff = ( (log << forward) - (log << backward) ) / ( 2 * eps);
	  // auto diff = (log << forward) / eps;
	  
	  return diff(at);
	}
	

      };


      struct pull {

	push impl;
	math::lie::group<domain> lie;
	
	pull(const numerical& of, const domain& at) 
	  : impl( d(of)(at) ),
	    lie( math::lie::of(at) )
	{

	}
	
	mutable typename math::lie::group<domain>::coalgebra res;
	// mutable typename math::lie::group<domain>::algebra delta;
	
	const typename math::lie::group<domain>::coalgebra& operator()
	(const typename math::lie::group<range>::coalgebra& f) const {
	  
	  
	  res = lie.coalg().zero();
	  // delta = lie.alg().zero();

	  // // partial differentials
	  // for(natural i = 0, n = lie.coalg().dim(); i < n; ++i) {
	    
	  //   lie.alg().coord(i, delta) = 1;
	  //   lie.coalg().coord(i, res) = math::euclid::pair(f, impl(delta));
	  //   lie.alg().coord(i, delta) = 0;
	  // }

	  auto task = [&](natural start, natural end) {
	    typename math::lie::group<domain>::algebra delta = lie.alg().zero();
	    auto copy = impl;

	    for(natural i = start; i < end; ++i) {
	      lie.alg().coord(i, delta) = 1;
	      lie.coalg().coord(i, res) = math::euclid::pair(f, copy(delta));
	      lie.alg().coord(i, delta) = 0;
	    }
	  };

	  // task(0, lie.coalg().dim()); 
	  core::parallel(0, lie.coalg().dim(), task);

	  return res;
	}
	
      };
      
    };

    template<class F>
    numerical<F> num(const F& f, math::real eps = 1e-5) { return {f, eps}; }

  }
}

#endif
