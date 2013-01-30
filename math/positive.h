#ifndef MATH_POSITIVE_H
#define MATH_POSITIVE_H

#include <math/real.h>
#include <math/func/id.h>
#include <math/func/exp.h>

namespace math {
  
  template<class U = real>
  struct positive {
    U value;
  };


  template<class U>
  struct structure<Lie< positive<U> > > {
    typedef positive<U> group;

    typedef U algebra;
    typedef U coalgebra;

    group identity() const { return { 0.0 }; }
    group inverse(const group& x) const { return { -x.value }; }
    group product(const group& x, const group& y) const { return { x.value + y.value }; }
    
    structure(const group&) { }
    structure() { }
    
    struct adjoint : func::id<algebra> { 
      adjoint(const group& ) { };
    };

    struct coadjoint : func::id<coalgebra> { 
      coadjoint(const group& ) { };
    };

    struct exponential  {
      
      typedef algebra domain;
      typedef group range;

      exponential( const Lie<group>& ) { }

      range operator()(const domain& x) const { return {x}; }

      T<range> diff(const T<domain>& dx) const { 
	return math::body( group{dx.at()}, dx.body() );
      }

      coT<domain> diffT(const pullT<exponential>& fx) const {
	return math::cobody(fx.at(), fx.body());
      }

      
    };

    struct logarithm  {
      
      typedef algebra range;
      typedef group domain;

      logarithm( const Lie<group>& ) { }

      range operator()(const domain& x) const { return x.value; }

      T<range> diff(const T<domain>& dx) const { 
	return math::body( dx.at().value, dx.body() );
      }

      coT<domain> diffT(const pullT<logarithm>& fx) const {
	return math::cobody(fx.at(), fx.body());
      }
      
    };

    Euclid<algebra> alg() const { return {}; }
    Euclid<coalgebra> coalg() const { return {}; }

  };

  template<class U = real>
  struct to_real {
    
    typedef positive<U> domain;
    typedef U range;

    range operator()(const domain& x) const { return func::exp()(x.value); }

    T<range> diff(const T<domain>& dx) const { 
      return func::exp()(math::body(dx.at().value, dx.body()) );
    }

  };


  template<class U>
  bool nan( const positive<U>& x ) {
    return nan( func::exp()(x.value) );
  }

}

#endif
