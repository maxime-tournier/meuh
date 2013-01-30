#include "symmetric.h"

#include <math/mat.h>
#include <math/form.h>

#include <math/func/tuple.h>
#include <math/func/compose.h>
#include <math/func/euclid.h>
#include <math/func/SE3.h>
#include <math/func/lie.h>
#include <math/func/vector.h>
#include <math/func/exp.h>
#include <math/func/constant.h>

#include <math/func/polynomial.h>

namespace tool {
  
  using namespace math;

  Lie<symmetric::dof_type> symmetric::lie() { return { Lie<SO3>(), Lie<vec2>() } ; }


  struct coeff_product {
    typedef vec3 domain;
    typedef real range;

    range operator()(const domain& x) const {
      return x(0) * x(1) * x(2);
    }

    T<range> diff(const T<domain>& dx) const {
      return math::body( (*this)(dx.at()),
			   dx.at()(1) * dx.at()(2) * dx.body()(0)
			 + dx.at()(0) * dx.at()(2) * dx.body()(1)
			 + dx.at()(0) * dx.at()(1) * dx.body()(2));
    }
  };

  using func::operator<<;
  static const auto dofs = func::tuple_get<0, symmetric::domain>();
  static const auto point = func::tuple_get<1, symmetric::domain>();

  // static const auto rotation = func::tuple_get<0, symmetric::dof_type>() << dofs;
  static const auto rotation = func::const_over<symmetric::domain>(SO3::identity());
  
  static const auto lambda = func::tuple_get<1, symmetric::dof_type>() << dofs;
  
  static const auto local = func::apply_rotation<>() << func::join( func::lie::inverse<SO3>() << rotation , point );
  
  //  static const auto weight_impl = coeff_product () << local;
  
  static const auto weight_impl = func::const_over<symmetric::domain, real>(1.0);
  
  // static const auto prod = func::euclid::bi_scalar<symmetric::dof_type>() 
  // 						<< func::join( symmetric::weight(), dofs );
  
  
  struct traceless { 
    typedef vec2 domain;
    typedef vec3 range;
    
    range operator()(const domain& x) const {
      return range(x(0), x(1), - ( x(0) + x(1) ) );
    }

    T<range>  diff(const T<domain>& dx) const {
      return math::body( (*this)(dx.at()),
			 (*this)(dx.body()) );
    }
  };

  static const auto exp3 = func::vec<3>( func::exp() );
  
  // cwise product
  struct cwise_product {

    typedef std::tuple<vec3, vec3> domain;
    typedef vec3 range;

    range operator()(const domain& g) const {
      return std::get<0>(g).cwiseProduct( std::get<1>(g));
    }
    
    T<range> diff(const T<domain>& dg) const {
      range at;
      Lie<range>::algebra body;

      static const auto lhs = func::tuple_get<0, domain>();
      static const auto rhs = func::tuple_get<1, domain>();
      
      at.each([&](natural i) {
	  const auto fun = func::euclid::bi_scalar<real>() << func::join( func::coord(i, at) << lhs,
									  func::coord(i, at) << rhs );
	  const T<real> dxi = fun.diff(dg);
	  
	  at(i) = dxi.at();
	  body(i) = dxi.body();
	});

      return math::body(at, body);
    }
    
  };

  // exp( lambda * mu(x) )
  static const auto scaling = exp3 << traceless() 
				      << func::euclid::bi_scalar<vec2>() << func::join( symmetric::weight(), lambda );
  
  static const auto full = func::apply_rotation<>() << func::join( rotation, cwise_product() << func::join( scaling, local ) );
  
  
  symmetric::weight::range symmetric::weight::operator()(const domain& x) const {
    return weight_impl(x);
  }

  T<symmetric::weight::range> symmetric::weight::diff(const T<domain>& dx) const {
    return weight_impl.diff(dx);
  }
  
  symmetric::range symmetric::operator()(const domain& x) const {
    return full(x);
  }

  T<symmetric::range> symmetric::diff(const T<domain>& dx) const {
    return full.diff(dx);
  }


  
  symmetric::field::range symmetric::field::operator()(const domain& x) const {
    const auto fun = symmetric() << func::join( func::const_over<domain>(at), func::id<domain>() );
    return fun(x);
  }

  T<symmetric::field::range> symmetric::field::diff(const T<domain>& dx) const {
    const auto fun = symmetric() << func::join( func::const_over<domain>(at), func::id<domain>() );
    return fun.diff(dx);
  }


  symmetric::map::range symmetric::map::operator()(const domain& x) const {
    const auto fun = symmetric() << func::join( func::id<domain>(), func::const_over<domain>(at) );
    return fun(x);
  }

  T<symmetric::map::range> symmetric::map::diff(const T<domain>& dx) const {
    const auto fun = symmetric() << func::join( func::id<domain>(), func::const_over<domain>(at)  );
    return fun.diff(dx);
  }
  


}
