#ifndef MATH_FUNC_TANGENT_H
#define MATH_FUNC_TANGENT_H

#include <math/types.h>
#include <math/func/alias.h>
#include <math/func.h>
#include <math/func/numerical.h>
#include <math/func/partial.h>
#include <math/func/ref.h>


namespace math {

  namespace func {

    // tangent map: (q, v) -> d(f)(q)(v)
    template<class F>
    struct tangent_map {

      F of;
      
      typedef typename traits<F>::domain of_domain;
      typedef typename traits<F>::range of_range;
      
      typedef std::tuple< of_domain, typename math::lie::group<of_domain>::algebra > domain;
      typedef typename math::lie::group<of_range>::algebra range;

      range operator()(const domain& x) const {
	return d(ref(of))(std::get<0>(x) )(std::get<1>(x));
      }
      
      
      struct pull {
	
	// TODO don't neglect second order terms !
	typename traits<F>::pull impl;
	math::euclid::space< typename math::lie::group<of_domain>::coalgebra > coalg;
	
	typedef numerical< compose<tangent_map, partial<0, domain> > > help_type;
	
	// const typename traits<help_type>::pull help;;
	
	
	pull(const tangent_map& of, const domain& at)
	  : impl( dT( ref(of.of) )( std::get<0>(at) ) ),
	    coalg( math::lie::of( std::get<0>(at)).coalg() )// ,
	    // help( dT( num( of << part<0>(at) ) )( std::get<0>(at) ) )
	{
	  std::get<0>( res ) = coalg.zero();
	}

	mutable typename math::lie::group<domain>::coalgebra res;
	typename math::lie::group<domain>::coalgebra zero;
	
	const typename math::lie::group<domain>::coalgebra& operator()
	(const typename math::lie::group<range>::coalgebra& f) const {
	  
	  std::get<1>( res ) = impl(f);
	  return res;
	}

      };

    };

    template<class F>
    tangent_map<F> tangent(const F& f) { return {f}; }


  }
}

#endif
