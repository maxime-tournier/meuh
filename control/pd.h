#ifndef CONTROL_PD_H
#define CONTROL_PD_H

#include <math/lie.h>
#include <math/euclid.h>
#include <math/tangent.h>
#include <math/func/lie.h>
#include <math/func/lie/translation.h>
#include <math/func/compose.h>
#include <math/covector.h>

#include <iostream>

namespace control {

  // see Bullo & Murray (95 ?), PD control on SE(3)
  template<class G>
  class pd {
    typedef typename math::Lie<G>::algebra algebra;
    typedef typename math::Euclid<algebra>::field field;
    
    const math::Lie<G> lie;
    
  public:
    field p, d;
    
    pd(field p = 1, field d = 1, 
       const math::Lie<G>& lie = math::Lie<G>())
      : lie(lie), p(p), d(d) { }
    
    // returns desired pos as a tangent vector from current pos
    algebra operator()(const math::T<G>& dstate,
		       const G& target) const {
      using namespace math;
      
      using lie::operator*;
      using func::operator<<;
      const T<G> error = func::lie::L( lie::inv(target) ).diff( dstate );
      
      using euclid::operator+;
      using euclid::operator*;

      return -p * lie.log()(error.at()) - d * error.body();
    }


     // returns desired pos as a tangent vector from current pos
    algebra operator()(const math::T<G>& dstate,
		       const math::T<G>& target) const {
      using namespace math;
      
      using lie::operator*;
      using func::operator<<;
      
      const auto f = func::lie::prod( func::lie::inverse<G>(), func::id<G>());
      
      const T<G> error = f.diff( tuple::merge(dstate, target) );
      
      using euclid::operator+;
      using euclid::operator*;

      return -p * lie.log()(error.at()) + -d * error.body();
    }

    
  };







}


#endif
