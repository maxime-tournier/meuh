#ifndef MATH_GAUSS_NEWTON_H
#define MATH_GAUSS_NEWTON_H

#include <math/func.h>
#include <math/minres.h>
#include <math/func/euclid.h>

namespace math {

	// damped gauss-newton

	template<class F>
	struct gauss_newton {
		typedef typename func::traits<F>::domain domain;
		typedef typename func::traits<F>::range range;
    
		const F f;
    
		const lie::group<domain> dmn;
		const lie::group<range> rng;

		iter outer, inner;
		real eps;

		gauss_newton(const F& f,
		             const lie::group<domain>& dmn,
		             const lie::group<range>& rng )
			: f(f),
			  dmn(dmn),
			  rng(rng),
			  eps(0)
		{
			inner.bound = -1;
			inner.epsilon = math::epsilon;

			outer.bound = -1;
			outer.epsilon = math::epsilon;
		}

    
		// don't assemble jacobians
		void sparse(domain& x, const range& rhs) const {
      
			auto residual = rng.log() << func::lie::L( rng.inv( rhs ) ) << ref(f);
			auto error2 = func::norm2( residual );
      
			vec b;
			b.resize(rng.alg().dim());
      
			typename lie::group<range>::algebra r = residual(x);
			typename lie::group<domain>::algebra delta = dmn.alg().zero();
      
			// TODO should we take log derivative into account ?
			outer.go( [&] {

					auto J = func::vectorize( d(ref(f))(x) );
					auto JT = func::vectorize( dT(ref(f))(x) );
	  
					auto JTJ = [&](const vec& x) {
						return JT( J( x ) );
					};

					rng.alg().get(b, r);
					dmn.alg().get(delta, math::minres(JTJ).solve(-JT(b), inner, vec(), -eps));
	  
					x = dmn.prod(x, dmn.exp()( delta ) );
					r = residual(x);
	  
					return rng.alg().norm(r);
				});

		}
    
		// TODO dense version lol

	};
  
	template<class F>
	gauss_newton<F> make_gauss_newton(const F& f,
	                                  const lie::group< typename func::traits<F>::domain >& dmn = lie::group< typename func::traits<F>::domain >(),
	                                  const lie::group< typename func::traits<F>::range >& rng = lie::group< typename func::traits<F>::range >() ) {
		return {f, dmn, rng};
	}
  
}



#endif
