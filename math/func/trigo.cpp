#include "trigo.h"

#include <cmath>

#include <boost/math/special_functions/sinc.hpp>

namespace math {
  namespace func {

    real sinc( real n ) {
      return boost::math::sinc_pi(n);
      // if ( std::abs(n) < epsilon ) {
      // 	return 1; 		// TODO better with TAYLOR
      // } else {
      // 	return std::sin( n ) / n;
      // }
      
    }
    
    real cosm1c( real n ) {
      if( std::abs(n) > epsilon ) {
	return (std::cos(n) - 1) / n;
      } else {
	// taylor
	real n2 = n * n;
	
	return -n2 + (n2 * n2) / 24; 
      }
      return boost::math::sinc_pi(n);
    }
    

    // real asinc( real n ) {
    //   if ( std::abs(n) < epsilon ) {
    // 	return 1; 		// TODO better with TAYLOR
    //   } else {
    // 	return std::asin( n ) / n;
    //   }
    // }
    
  }
}

