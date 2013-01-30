#ifndef MATH_EUCLID_BASIS_H
#define MATH_EUCLID_BASIS_H

#include <math/euclid.h>

namespace math {
  namespace euclid {

    namespace basis {

      template<class E>
      typename Euclid<E>::coordinates coordinates(unsigned int i, const Euclid<E>& euclid = Euclid<E>() ) {
	assert( i < euclid.dim() );
	
	typename Euclid<E>::coordinates res = euclid.coordinates( euclid.zero() );
	res(i) = 1;
	return res;
      }


      
      
      template<class E>
      E element(unsigned int i, const Euclid<E>& euclid = Euclid<E>() ) {
	
	return euclid.elem( coordinates(i, euclid) );
      }

    }

  }
}


#endif
