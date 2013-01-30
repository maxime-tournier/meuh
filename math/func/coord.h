#ifndef MATH_FUNC_COORD_H
#define MATH_FUNC_COORD_H

#include <math/vector.h>
#include <math/lie.h>

#include <cassert>

namespace math {
  namespace func {

    template<class> struct coordinate;

    template<class G, int M >
    struct coordinate< math::vector<G, M> >{

      typedef math::vector<G, M> domain;
      typedef G range;

      natural i;
      
      coordinate(natural i) : i(i) { }
      
      range operator()(const domain& g) const {
	assert( +i < g.rows() );
	return g(i);
      }


      struct push : base< coordinate< typename math::lie::group<domain>::algebra > > {
	push(const coordinate& of, const domain& )
	  : push::base(of.i) {

	}

      };

      struct pull  {
	
	natural i;
	
	mutable typename math::lie::group<domain>::coalgebra res;

	pull( const coordinate& of, const domain& at)
	  : i(of.i) {
	  res = math::lie::of(at).coalg().zero();
	}
	
	const typename math::lie::group<domain>::coalgebra& operator()(const typename math::lie::group<range>::coalgebra& f) const {
	  res(i) = f;
	  return res;
	}
	
      };

      
    };

    
    template<class Vector>
    coordinate<Vector> coord(natural i) { return {i}; }
    
  }
}


#endif
