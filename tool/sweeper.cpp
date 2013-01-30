#include "sweeper.h"

#include <math/func/tuple.h>
#include <math/func/euclid.h>
#include <math/func/SE3.h>

#include <math/func/diff.h>
#include <math/func/exp.h>

namespace tool {
  using namespace math;
  
  static const auto dof = func::tuple_get<0, sweeper::domain >();
  static const auto point = func::tuple_get<1, sweeper::domain >();

  static const auto center = func::tuple_get<0, sweeper::dof_type>() << dof;
  static const auto axis = func::tuple_get<1, sweeper::dof_type>() << dof;

  using func::operator*;
  static const auto mu = func::exp() << (-0.1 * func::euclid::norm2<vec3>());
  
  static const auto twist = func::euclid::bi_scalar<vec3>() 
    << func::join( mu << point, axis );  
  
  static const auto rotation = Lie<SO3>().exp() << twist;

  using func::operator-;
  using func::operator+;

  static const auto local = point - center;
  
  static const auto final = (center + (func::apply_rotation<>() << func::join(rotation, local) ) );
  static const auto dfinal = db( final );
  
  sweeper::range sweeper::operator()(const domain& x) const {
    return final(x);
  }
  
  sweeper::push::push(const sweeper&, const domain& at) : at(at) { }
  
  Lie<sweeper::range>::algebra sweeper::push::operator()(const Lie<domain>::algebra& dx) const {
    return dfinal(at)(dx);
  }
  

}
