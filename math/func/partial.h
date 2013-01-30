#ifndef MATH_FUNC_PARTIAL_H
#define MATH_FUNC_PARTIAL_H

// TODO rename part.h ?

#include <math/func/tuple.h>

namespace math {
  namespace func {

    template<int I, class > struct partial;

    template<int I, class ... Args>
    struct partial<I, std::tuple<Args...> > {
      
      typedef std::tuple<Args...> range;
      typedef typename std::tuple_element<I, range>::type domain;

      range at;

      range operator()(const domain& x) const {
	range res = at;
	std::get<I>(res) = x;
	return res;
      }


      struct push {
	typedef typename math::lie::group<range>::algebra algebra;
	mutable algebra res;

	push(const partial& of, const domain&) 
	  : res( math::lie::of(of.at).alg().zero() ) {
	  
	}

	const algebra& operator()(const typename math::lie::group<domain>::algebra& v) const {
	  std::get<I>(res) = v;
	  return res;
	}

      };

      struct pull : tuple_get<I, typename math::lie::group<range>::coalgebra> {
	pull(const partial& , const domain& ) { };
      };
      
      
    };

    template<int I, class ... Args>
    partial<I, std::tuple<Args...> > part(const std::tuple<Args...>& at) { return {at}; }
    
  }
}



#endif
