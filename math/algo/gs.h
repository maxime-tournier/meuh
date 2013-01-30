#ifndef MATH_ALGO_GS_H
#define MATH_ALGO_GS_H

#include <math/algo/iter.h>
#include <math/matrix.h>
#include <math/vec.h>


#include <math/error.h>

#include <cassert>

namespace math {
  namespace algo {
 
    namespace proj {
    
      struct none {
	real operator()(const vec& data, natural i ) const;
      };
      
  
      struct pos {
	real operator()(const vec& data, natural i) const;
      };


      struct neg {
	real operator()(const vec& data, natural i) const;
      };

      struct bounds {
	const vec& low;
	const vec& up;


	bounds(const vec& low, const vec& up);
	real operator()(const vec& data, natural i) const;
      };
      
    }
    
    

    struct gs {

      const vec diag;
      const mat iter;
      
      gs(const mat& M, math::real eps = 0);
      
      void check(const vec& x) const;
      real step(const vec& x, const vec& q, natural i) const;
      
      template<class Proj>
      vec solve(const vec& q, const Proj& proj,
		const algo::stop& stop, vec x = vec()) const {
	
	const natural n = q.rows();
	check(q);
            
	if( x.empty() ) x = vec::Zero(n);
	check(x);
      
	algo::iter_eps(stop, [&](real& eps) {
	    eps = 0;
	    // Gauss-Seidel iteration
	    for(natural i = 0; i < n; ++i) {
	      const real x_i = x(i);
	    
	      x(i) = step(x, q, i);
	      x(i) = proj(x, i);  
	      
	      const real diff = x(i) - x_i;
	      eps += diff * diff;
	    }
	  
	    eps = std::sqrt(eps);
	  });
      
       
	return x;
	
      }
      
    };
    
  }
}

#endif
