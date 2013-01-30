#ifndef MATH_FUNC_LIE_SUBGROUP_H
#define MATH_FUNC_LIE_SUBGROUP_H

#include <math/lie.h>

namespace math {
  namespace func {
    namespace lie {

      // one parameter subgroup
      template<class G> 
      struct subgroup  {
	typedef typename math::lie::group<G>::algebra algebra;
	typedef typename math::lie::group<G>::coalgebra coalgebra;
            

	algebra direction;
	math::lie::group<G> lie;
	
	subgroup(const algebra& direction, 
		 const math::lie::group<G>& lie = math::lie::group<G>() ) 
	  : direction(direction),
	    lie(lie)
	{ }
      
	typedef typename math::euclid::space< algebra >::field domain;
	typedef G range;
	
	range operator()(const domain& t) const {
	  return lie.exp()( lie.alg().scal(t, direction ) );		       
	}

	struct push {
	  
	  const math::euclid::space<algebra> alg;
	  const algebra direction;
	  
	  push(const subgroup& of, const domain& ) 
	    : alg( of.lie.alg() ),
	      direction(of.direction) {

	  }

	  algebra operator()(const domain& v) const {
	    return alg.scal(v, direction);
	  }
	  
	};


	struct pull {
	  const math::euclid::space<coalgebra> coalg;
	  const coalgebra direction;

	  pull(const subgroup& of, const domain& ) 
	    : coalg(of.lie.coalg()), 
	      direction( math::euclid::pair(of.direction) ) {

	  }

	  domain operator()(const coalgebra& f) const {
	    return coalg.dot(direction, f);
	  }
	  
	};

        
      };



    }
  }
}
   

#endif
