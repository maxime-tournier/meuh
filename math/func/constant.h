#ifndef MATH_FUNC_CONSTANT_H
#define MATH_FUNC_CONSTANT_H

#include <math/lie.h>

namespace math {
  namespace func {

    template<class Domain, class Range = Domain>
    struct constant {
      
      typedef Domain domain;
      typedef Range range;

      constant( const range& value ) : value(value) { }
      constant( range&& value ) : value( std::move(value) ) { }
      
      range value;
    
      range operator()(const domain&) const { return value; }
      
      struct push : base< constant< typename math::lie::group<Domain>::algebra,
				    typename math::lie::group<Range>::algebra > >
      {
	push(const constant& of, const domain&)
	  : push::base( math::lie::of(of.value).alg().zero() ) {

	}
      };

      struct pull : base< constant< typename math::lie::group<Range>::coalgebra,
				    typename math::lie::group<Domain>::coalgebra > >
      {
	pull(const constant&, const domain& at)
	  : pull::base( math::lie::of(at).coalg().zero() ) {
	  
	}
	
      };
    };

    
    template<class Domain, class Range>
    constant<Domain, Range> cst(const Range& x) { return {x}; }
    
    template<class Range>
    constant<Range, Range> val(const Range& x) { return {x}; }
    
  }

}

#endif
