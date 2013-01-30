#ifndef MATH_FUNC_SELECT_H
#define MATH_FUNC_SELECT_H

#include <math/vector.h>
#include <vector>

namespace math {
  namespace func {

    template<class G>
    struct select {
      
      typedef std::vector<natural> index_type;
      index_type index;
      
      typedef math::vector<G> domain;
      typedef math::vector<G> range;
      
      math::lie::group< domain > dmn;

      select(const index_type& index,
	     const math::lie::group< domain >& dmn = math::lie::group<domain>() )
	: index(index),
	  dmn(dmn)
      {

      }


      range operator()(const domain& x) const {
	range res;
	res.resize( index.size() );

	res.each([&](natural i) {
	    res(i) = x(index[i]);
	  });

	return res;
      }


      struct push : base< select< typename math::lie::group<G>::algebra > > {

	push(const select& of, const domain& at) 
	  : push::base(of.index) {

	}

      };

      struct pull  {
	const index_type index;
	const math::euclid::space< typename math::lie::group<domain>::coalgebra > coalg;
	
	pull(const select& of, const domain& at) 
	  : index(of.index),
	    coalg(of.dmg.coalg())
	{
	  
	}

	typename math::lie::group<domain>::coalgebra operator()(const typename math::lie::group<range>::coalgebra& f) const {

	  // HURRR
	  auto res = coalg.zero(); 
	  
	  for(natural i = 0, n = index.size(); i < n; ++i) {
	    // FIXME: use domain coalgebra to do the sum
	    res(index[i]) += f(i);
	  }

	  return res;
	} 


      };



    };

  }
}
#endif
