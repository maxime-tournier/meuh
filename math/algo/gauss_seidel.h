#ifndef MATH_ALGO_GAUSS_SEIDEL_H
#define MATH_ALGO_GAUSS_SEIDEL_H

#include <math/algo/iter.h>
#include <math/matrix.h>
#include <math/vec.h>


#include <math/error.h>

#include <cassert>

namespace math {
  namespace algo {
 
    namespace proj {
    
      struct none {
	real operator()(const vec& data, natural i ) const { 
	  assert( int(i) < data.rows() &&  "bad index" );
	  return data(i); 
	}
      };

  
      struct pos {
	real operator()(const vec& data, natural i) const {
	  assert( int(i) < data.rows() &&  "bad index" );
	
	  const real val = data(i);
	  return val < 0 ? 0 : val;
	}
      
      };
      
    }
    
    

    struct gs {

      const vec diag;
      const mat iter;
      
      pgs(const mat& M, math::real eps = 0);
      
      void check(const vec& x) const;
      real step(const vec& x, const vec& q, natural i) const;
      
      vec solve(const vec& rhs, const algo::stop& stop = algo::stop(), vec init = vec()) const; 
      
      template<class Proj>
      vec solve(const vec& q, const Proj& proj,
		const algo::stop& stop, vec x = vec()) const {
	
	
      }
      
    };
    
  }
}

#endif
