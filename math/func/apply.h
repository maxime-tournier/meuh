#ifndef MATH_FUNC_APPLY_H
#define MATH_FUNC_APPLY_H

#include <math/func/partial.h>
#include <math/func/numerical.h>
#include <math/func/compose.h>
#include <math/func/euclid.h>
#include <math/func/tuple.h>

namespace math {
  namespace func {

    // computes z = f(x)(y)

    // handles functional derivative by numerically differentiating
    // the function application
    template<class F>
    struct application {

      const F f;
      
      typedef typename traits<F>::domain x_type;
      typedef typename traits<F>::range fx_type;
      
      typedef typename traits<fx_type>::domain y_type;
      typedef typename traits<fx_type>::range z_type;
      
      typedef std::tuple<x_type, y_type> domain;
      typedef z_type range;

      typedef tuple_get<0, domain> x;
      typedef tuple_get<1, domain> y;
      
      range operator()(const domain& p) const {
	return f( x()(p) ) (y()(p) );
      }     


      struct push {
	
	typedef typename traits<fx_type>::push dfx_dy_type;
	const dfx_dy_type dfx_dy;
	
	// typedef numerical< compose< application, partial<0, domain> > > num_type;
	// typedef typename traits<num_type>::push dfxy_dx_type;
	
	// const dfxy_dx_type dfxy_dx;

	// what is this i dont even
	push(const application& of, const domain& at)
	  : dfx_dy( d(of.f( x()(at) ) )( y()(at) ) )// ,
	  
	    // dfxy_dx( d( num( of << part<0>(at) ) )( x()(at) ) )
	{

	  
	  // warning: the above of << part<0>(at) tries to instanciate
	  // traits<application>::push, which fails (possibly since we
	  // are defining this type here, though this is the body of
	  // ctor and should probably be safe)
	}
	
	typename math::lie::group<range>::algebra operator()
	(const typename math::lie::group<domain>::algebra& v) const {
	  using math::func::operator+;
	  auto impl = // (dfxy_dx << func::get<0>(v)) +
	    (dfx_dy << func::get<1>(v));
	  return impl(v);
	}
	
      };

      struct pull {
	
	typedef typename traits<fx_type>::pull dTfx_dy_type;
	const dTfx_dy_type dTfx_dy;
	
	const domain at;

	// typedef numerical< compose< application, partial<0, domain> > > num_type;
	// typedef typename traits<num_type>::push dfxy_dx_type;
	
	// const dfxy_dx_type dfxy_dx;

	// what is this i dont even
	pull(const application& of, const domain& at)
	  : dTfx_dy( dT(of.f( x()(at) ) )( y()(at) ) ),
	    at(at)
	    // ,
	    
	    // dfxy_dx( d( num( of << part<0>(at) ) )( x()(at) ) )
	{

	  // warning: the above of << part<0>(at) tries to instanciate
	  // traits<application>::push, which fails (possibly since we
	  // are defining this type here, though this is the body of
	  // ctor and should probably be safe)
	}
	
	typename math::lie::group<domain>::coalgebra operator()
	(const typename math::lie::group<range>::coalgebra& f) const {
	  return dT( y() )(at)( dTfx_dy(f) );
	}
	
      };


      
    };

    template<class F>
    application<F> apply(const F& f) { return {f}; }
    

    // template<class F>
    // struct traits< application<F> > {
    //   typedef typename application<F>::range range;
    //   typedef typename application<F>::domain domain;

    //   typedef typename application<F>::push push;
    //   typedef default_pull<F> pull;
    // };

  }
}

#endif
