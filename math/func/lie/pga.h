#ifndef MATH_FUNC_LIE_PGA_H
#define MATH_FUNC_LIE_PGA_H

#include <math/lie.h>
#include <math/func/coord.h>
#include <math/func/lie.h>
#include <math/func/lie/translation.h>

#include <math/func/lie/ccsk.h>
#include <math/func/lie/ccfk.h>


namespace math {
  namespace func {
    
    namespace lie {

      template<class G, template<class> class coordinates = ccfk >
      struct pga {
      
	typedef typename Lie<G>::algebra algebra;

	const G& mean;
	const math::vector<algebra>& geodesics;
	const Lie<G> lie;
	const natural n;
	
	pga(const G& mean, 
	    const math::vector<algebra>& geodesics,
	    natural nn = 0,
	    const Lie<G>& lie = Lie<G>() ) 
	  : mean(mean), 
	    geodesics(geodesics),
	    lie(lie),
	    n( nn ? nn : geodesics.rows() ) { 
	  assert(n && int(n) <= geodesics.rows() );
	}
	
	typedef typename Euclid<algebra>::field field;
	typedef math::vector<field> domain;
	typedef G range;
	
	range operator()(const domain& x) const {
	  assert(x.rows() == int(n) );
	  
	  return (L(mean) << coordinates<G>(geodesics, n, lie))(x);
	}
	
	T<range> diff(const T<domain>& dx) const {
	  assert(dx.at().rows() == int(n) );
	  assert(dx.body().rows() == int(n) );
	  
	  return d(L(mean) << coordinates<G>(geodesics, n, lie)) (dx);
	}

	coT<domain> diffT(const pullT<pga>& fx) const {
	  assert(fx.at().rows() == int(n) );
	  assert( math::euclid::dim(fx.body()) == lie.coalg().dim() );
	  
	  return  math::pull(L(mean) << coordinates<G>(geodesics, n, lie), fx.at(), fx.body());
	}
	
      };

      
    }
  }
}

#endif
