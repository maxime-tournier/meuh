#ifndef MATH_REAL_H
#define MATH_REAL_H

#include <math/lie.h>
#include <math/lie/flat.h>

#include <cassert>

namespace math {

  namespace euclid {
    template<>
    struct traits<real>  {
      typedef real field;
      typedef real dual;

      real zero() const { return 0.0; }
      real minus(const real& x) const { return -x; }
      real sum(const real& x, const real& y) const { return x + y; }
      real scal(real lambda, const real& x) const { return lambda * x; }
      
      space<dual> operator*() const { return {}; }

      natural dim() const { return 1; }

      traits(const real& ) {}
      traits() { }
      
      field& coord(natural i, real& x) const {
	assert( i == 0 ); core::use( i );
	return x;
      }

      const field& coord(natural i, const real& x) const {
	assert( i == 0 ); core::use( i );
	return x;
      }

      
    };
  }

  namespace lie {

    template<>
    struct traits<real> : flat<real> {
      traits(const real& ) { }
      traits()  { }
    };

    
  }
  

  template<>
  struct precision<double> {
    static constexpr double epsilon = 1e-16; 
    static constexpr double inf = 1e69;
  };
    
  const real epsilon = precision<real>::epsilon;
  const real inf = precision<real>::inf;
  
  // useful one-liners
  inline real abs(const real& x) { return x < 0 ? -x : x; }
  inline real sign(const real& x) { return x < 0 ? -1.0 : 1.0; }

  // division safety
  inline bool safe(const real& x) { return abs(x) >= epsilon; }
  
}
  

#endif
