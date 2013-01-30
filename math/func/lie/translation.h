#ifndef MATH_FUNC_LIE_TRANSLATION_H
#define MATH_FUNC_LIE_TRANSLATION_H

#include <math/lie.h>

namespace math {
  namespace func {
    namespace lie {
      
      template<class G>
      struct left {
	math::lie::group<G> lie;
	G by;

	left(const G& by) 
	: lie( math::lie::of(by)),
	  by(by) {
	}

	G operator()(const G& g) const {
	  return lie.prod(by, g);
	}
	
	
	struct push : id< typename math::lie::group<G>::algebra > {
	  push(const left&, const G& )  { }
	};
	
	struct pull : id< typename math::lie::group<G>::coalgebra > {
	  pull(const left&, const G& )  { }
	};
	
      };

      template<class G>
      struct right {
	math::lie::group<G> lie;
	G by;

	right(const G& by) 
	: lie( math::lie::of(by)),
	  by(by) {
	}
				 
	
	G operator()(const G& g) const {
	  return lie.prod(g, by);
	}
	
	
	struct push : math::lie::group<G>::adjoint {

	  push(const right& of, const G&) 
	    : math::lie::group<G>::adjoint( of.lie.inv(of.by) ) {

	  }
	  
	};

	struct pull : math::lie::group<G>::coadjoint {

	  pull(const right& of, const G&) 
	    : math::lie::group<G>::coadjoint( of.lie.inv(of.by) ) {
	    
	  }
	  
	};
	
	
      };

	
      
   

      template<class G>
      left<G> L(const G& g) { return {g}; }

      template<class G>
      right<G> R(const G& g) { return {g}; }

      
    }
  }
}


#endif
