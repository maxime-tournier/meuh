#include "minres.h"

#include <cmath>

#include <math/error.h>
#include <core/macro/derp.h>
#include <cassert>

namespace math {


  namespace impl {

    void minres::init(const vec& rr, math::real threshold) {
      natural n = rr.rows();
      
      auto zero = vec::Zero(n);
	
      r = rr;
      assert(!nan(r));
      
      beta = r.norm();
      assert(!nan(beta));
      
      if( beta <= threshold ) {
	phi = 0;
	return;
      }
      
      v_prev = zero;
      v = r.normalized();

      assert( !nan(v) );
 
      v_next = zero;

      d = zero;
      d_prev = zero;

      phi = beta;
      tau = beta;
	
      delta_1 = 0;
      gamma_min = 0;

      norm = 0;
      cond = 0;

      c = -1;
      s = 0;

      eps = 0;
      k = 1;
    }





    void minres::sym_ortho(const real& a, const real& b,
			   real& c, real& s, real& r) {
      assert( !nan(a) );
      assert( !nan(b) );

      if( !b  ) {
	s = 0;
	r = abs( a );
      
	c = a ? sign(a) : 1.0;
      
      } else if ( !a ) {
	c = 0;
	s = sign(b);
	r = abs(b);
      } else {

	const real aabs = abs(a);
	const real babs = abs(b);
      
	if( babs > aabs ) {
	  const real tau = a / b;
	  assert( !nan(tau) );

	  s = sign( b ) / std::sqrt( 1 + tau * tau );
	  c = s * tau;
	  r = b / s;
	} else if( aabs > babs ) { 
	    const real tau = b / a;
	    assert( !nan(tau) );
	    
	    c = sign( a ) / std::sqrt( 1 + tau * tau );
	    s = c * tau;
	    r = a / c;
	}  else {
	  // core::log( DERP, "a:", a, "b:", b );

	  const real tau = b / a;
	  assert( !nan(tau) );
	    
	  c = sign( a ) / std::sqrt( 1 + tau * tau );
	  s = c * tau;
	  r = a / c;
	}
	
      }
   
      assert(! nan(c) );
      assert(! nan(s) );
      assert(! nan(r) );
 
    }
  
  }

}
 
