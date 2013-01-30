#ifndef MATH_FUNC_JACOBIAN_H
#define MATH_FUNC_JACOBIAN_H

#include <math/lie.h>

#include <math/func/push.h>
#include <math/func/ref.h>

#include <math/mat.h>

namespace math {
  namespace func {

    template<class F>
    struct jacobian {
      
      F of;

      typedef typename traits<F>::domain domain;
      typedef typename traits<F>::range range;
      
      typedef typename math::lie::group< domain >::algebra domain_algebra;
      typedef typename math::lie::group< range >::algebra range_algebra;
      
      math::euclid::space< domain_algebra > dmn;
      math::euclid::space< range_algebra > rng;
      
      natural m;
      natural n;
      
      jacobian(const F& of, 
	       const math::lie::group<domain>& dmn = math::lie::group<domain>(),
	       const math::lie::group<range>& rng = math::lie::group<range>())
	: of( of ),
	  dmn( dmn.alg() ),
	  rng( rng.alg() ),
	  m( this->dmn.dim() ),
	  n( this->rng.dim() )
      {
	
      }

      // TODO prealloc mat ?
      // TODO thread safety lol
      math::mat operator()(const domain& at) const {
	
	math::mat res;
	res.resize( n, m );
	
	domain_algebra body = this->dmn.zero();
	range_algebra d_of_dxi = this->rng.zero();
	
	auto d_of = d(ref(of))(at);
	
	// TODO fixme
	auto task = [&, body, d_of_dxi, d_of](natural start, natural end) mutable {
	    
	    for(natural i = start; i < end; ++i) {
	      this->dmn.coord(i, body) = 1;
	      
	      d_of_dxi = d_of( body );
	      
	      res.each_row([&](natural j) {
		  res(j, i) = this->rng.coord(j, d_of_dxi);
		});
	      
	      this->dmn.coord(i, body) = 0;
	    }
	    
	};
	task(0, m);
	// core::parallel(0, m, task);
	
	return res;
      }
      
    };
    
    template<class F>
    jacobian<F> J(const F& of, 
		  const math::lie::group< typename traits<F>::domain >& dmn = math::lie::group< typename traits<F>::domain >(),
		  const math::lie::group< typename traits<F>::range>& rng = math::lie::group< typename traits<F>::range>()) {
      return {of, dmn, rng};
    }


    

    
  }
}




#endif
