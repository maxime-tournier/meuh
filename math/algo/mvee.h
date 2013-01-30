#ifndef MATH_ALGO_MVEE_H
#define MATH_ALGO_MVEE_H

#include <math/mat.h>
#include <math/vec.h>
#include <math/algo/iter.h>

namespace math {
  namespace algo {
    
    struct mvee {
      
      struct ellipsoid {
	mat metric;
	vec center;

	ellipsoid(const mat& metric, const vec& center = vec() );
	
	bool contains(const vec& x) const; 
	real distance(const vec& x) const; 
	real distance_squared(const vec& x) const; 
	vec project(const vec& x) const;
	
	real volume() const;

      };

      // elements in cols !
      ellipsoid operator()(const mat& P, const stop& s = stop(), real fuzz = math::epsilon ) const;
      
    };
    

  }

}




#endif
