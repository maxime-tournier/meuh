#include "active_set.h"

#include <core/log.h>

namespace math {

  active_set::active_set(natural n, natural start) 
  {
    assert( start <= n );

    if( n ) {
      mask = vec::Ones(n);
      bilateral = vec::Zero(n);
    }
    
    if( start ) bilateral.head(start).setOnes();
  }
      

  bool active_set::update(const vec& primal, const vec& dual) {
    assert(primal.size() == dual.size());
    assert(primal.size() == mask.size());
    
    bool restart = false;

    for(natural i = 0, n = mask.size(); i < n; ++i) {
      if(!bilateral(i)){ 
	real old = mask(i);
	
	if( dual(i) <= 0 ) {
	  mask(i) = 0.0;
	}
      
	if( primal(i) < 0 ) {
	  mask(i) = 1.0;
	} 
	  
	// *primal* has the last word
	if(old != mask(i)) {
	  restart |= true;
	}
      }
    }
    
    // core::log("primal:", primal.transpose());
    // core::log("dual:", dual.transpose());
    // core::log("mask:", mask.transpose());
    
    return restart;
  }
  

  real active_set::step(const vec& old, const vec& desired) {
    // we need: alpha A (v - vold) >= b - A vold

    // A ( v - vold )
    vec delta = desired - old; 	
    
    real res = 1.0;
    
    delta.each([&](natural i) {
	
	if(delta(i) >= 0 ) return;

	real step = - old(i) / delta(i);
	
	if( step <= 1e-8 ) return;
	if( step >= 1 ) return;
	
	if( step < res ) res = step;
      });

    core::log("step:", res);
    return res;
  }

  vec active_set::filter(const vec& other) const {
    assert( !mask.empty() );
    return mask.cwiseProduct(other);
  }

}
