#ifndef MATH_FUNC_LIE_EXP_H
#define MATH_FUNC_LIE_EXP_H

#include <math/lie.h>
#include <math/tangent.h>
#include <math/pullback.h>

namespace math {
  namespace func {
    namespace lie {
      
      // // TODO obselete ! remove ! burrrrn !
      // template<class G>
      // struct exponential { 
	
      // 	exponential(const Lie<G>& lie = Lie<G>() ) : lie(lie) { }
	
      // 	const Lie<G> lie;

      // 	typedef typename Lie<G>::algebra domain;
      // 	typedef G range;

      // 	range operator()(const domain& v) const { return lie.exp()(v); }
      // 	T<range> operator()(const T<domain>& dv) const { return math::body( lie.exp(dv.at()), lie.dexp(dv.at(), dv.body())); }
      // 	coT<domain> operator()(const pullT<exponential>& fv) const { return math::cobody(fv.at(), lie.dexpT(fv.at(), fv.body())); }
	
      // };

      // template<class G>
      // exponential<G> exp(const Lie<G>& lie = Lie<G>() ) { return {lie}; }
      
    }
  }
}


#endif
