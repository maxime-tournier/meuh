#ifndef MATH_LIE_FLAT_H
#define MATH_LIE_FLAT_H

#include <math/lie.h>
#include <math/func/id.h>

namespace math {
  namespace lie {

    template<class E>
    struct flat {
      
      const euclid::space<E> impl;
      
      template<class ... Args>
      flat(Args&&... args) : impl(std::forward<Args>(args)...) { }
      flat() : impl() { }

      flat( const flat& ) = default;
      flat( flat&& ) = default;
      flat( const flat&& other) : impl( std::move(other.impl) ) { }

      typedef E algebra;
      typedef E coalgebra;

      struct adjoint :  func::id< algebra > {
	adjoint( const E& ) { } 
      };

      struct coadjoint :  func::id< coalgebra > {
	coadjoint( const E& ) { } 
      };

      struct exponential : func::id<E> {
	exponential( const group<E>& ) { }
      };
      
      struct logarithm : func::id<E> {
	logarithm( const group<E>& ) { }
      };
      
      

      auto id() const -> decltype( impl.zero() ) { 
	return impl.zero(); 
      }

      auto inv(const E& x) const -> decltype( impl.minus(x) ) { 
	return impl.minus( x );
      }

      auto prod(const E& x, const E& y) const -> decltype( impl.sum(x, y) ) { 
	return impl.sum( x, y );
      }

      euclid::space<algebra> alg() const { return impl; }
      
    };
    
  }
}


#endif
