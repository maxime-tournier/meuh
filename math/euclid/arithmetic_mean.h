#ifndef MATH_EUCLID_MEAN_H
#define MATH_EUCLID_MEAN_H

#include <math/euclid.h>
#include <math/vector.h>

namespace math {

  namespace euclid {

    template<class E>
    struct arithmetic_mean {
      const space<E> euclid;
      
      E operator()(const vector<E>& samples) const {
	const math::natural n = samples.rows();
	
	E sum = euclid.zero();
	 
	samples.each([&](natural i) {
	    using euclid::operator+;
	    sum = sum + samples(i);
	  });

	using euclid::operator/;
	return sum / n;
      }
      
      
    };

    template<class E>
    arithmetic_mean<E> mean(const space<E>& euclid = space<E>() ) { return { euclid }; }
    

  }
  
}



#endif
