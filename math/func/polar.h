#ifndef MATH_FUNC_POLAR_H
#define MATH_FUNC_POLAR_H

#include <math/vector.h>
#include <math/tuple.h>

#include <math/error.h>

namespace math {
  namespace func {

    // TODO extend to general Euclid case
    template<class U, int M>
    struct polar {
      
      // gives the polar representation of a vector x (||x||, x/||x||)
      typedef math::vector<U, M> domain;
      typedef std::tuple<U, domain> range;

      range operator()(const domain& x) const {
	const U theta = x.norm();

	// TODO use euclid::zero to do the zero check
	if( theta < precision<U>::epsilon ) throw math::error("can't get polar coordinates");
	
	const domain n = x / n;
	
	return std::make_tuple(theta, n);
      }

      // TODO pushforward !
      // T<range> operator()(const T<domain>& dx) const {
	
      // 	const auto at = operator()(dx.at());
	
      // 	using namespace std;
      // 	const U& theta = get<0>(at);
      // 	const domain& n = get<1>(at);
	
      // 	typename Lie<range>::algebra body;
	
      // 	get<0>(body) = n.dot(dx.body());
      // 	get<1>(body) = (dx.body() - n * get<0>(body)) / theta;

      // 	return math::body(at, body);
      // }


    };

  }
}
 
#endif

