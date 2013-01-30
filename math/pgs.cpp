#include "pgs.h"

namespace math {


  vec pgs::solve(const vec& q, iter it) const {

    vec res = vec::Zero(q.size());

    it.go([&] {

	real eps = 0;
	res.each([&](natural i) {
	
	    real old = res(i);
	    res(i) -=  (q(i) + M.col(i).dot(res)) / M(i, i);
	    
	    // projection
	    res(i) = std::max(0.0, res(i));
	    
	    real delta = res(i) - old;

	    eps += delta * delta;
	  });
	
	return std::sqrt( eps );
      });
    
    return res;
  }
  
  
}
