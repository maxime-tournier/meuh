#ifndef MATH_FUNC_LIE_CCSK_H
#define MATH_FUNC_LIE_CCSK_H

#include <math/func/lie.h>
#include <math/func/lie/subgroup.h>

namespace math { 
  namespace func {
    namespace lie {
      
      template<class G>
      struct ccsk {

	typedef typename math::lie::group<G>::algebra algebra;
	typedef typename math::euclid::space< algebra >::field field;
	
	typedef math::vector<field> domain;
	typedef G range;
	
	typedef product_vector<G> prod_type;
	typedef func::vector< subgroup<G> > exps_type;
	
	typedef compose< prod_type, exps_type> impl_type;
	impl_type impl;
	
	ccsk(const math::vector<algebra>& dirs,
	     natural n = 0,
	     const math::lie::group<G>& lie = math::lie::group<G>()) 
	  : impl( prod_type(n, lie),
		  exps_type( core::make_const_vector(n, [&](natural i) {
			return subgroup<G>(dirs(i), lie);
		      })) )
   	{ 
	  assert(n && (int(n) <= dirs.rows()) );
	}
	
	range operator()(const domain& x) const {
	  return impl(x);
	}
	

	struct push : base< typename traits<impl_type>::push > {

	  push(const ccsk& of, const domain& at) 
	    : push::base(of.impl, at) {

	  }

	};


	struct pull : base< typename traits<impl_type>::pull > {

	  pull(const ccsk& of, const domain& at) 
	    : pull::base(of.impl, at) {

	  }

	};

      };

    };
  }
}


#endif
