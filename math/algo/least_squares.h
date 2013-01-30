#ifndef MATH_ALGO_LEAST_SQUARES_H
#define MATH_ALGO_LEAST_SQUARES_H

#include <math/matrix.h>
#include <math/vec.h>

#include <math/algo/stop.h>

namespace math {
  namespace algo {

  
    // assumes full rank on the smallest dimension
    namespace least_squares {
      
      // gives the normal equations matrix
      mat normal(const mat& A);
      bool over( const mat& A);
      bool under( const mat& A);

      // general solve. can tweak normal matrix if needed.
      struct solve {
	const mat& original;
	const mat normal;
	
	solve(const mat& A);
	solve(const mat& A, const mat& normal);	

	vec operator()(const vec& b, const stop& s = stop() ) const;
	vec operator()( const vec& b, const vec& init, const stop& s  = stop() ) const;
    
      };
      
      // adds eps-identity to normal matrix 
      struct damped : solve{
	const real epsilon;
	
	damped(const mat& A, real epsilon = 1e-14); // HARDCODE
	
      };
  
    }
  
  
  }

}
#endif
