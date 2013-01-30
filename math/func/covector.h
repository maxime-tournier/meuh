#ifndef MATH_FUNC_COVECTOR_H
#define MATH_FUNC_COVECTOR_H

#include <math/covector.h>

#include <math/func.h>
#include <math/func/push.h>
#include <math/func/pull.h>

#include <core/const_vector.h>

namespace math {
  namespace func {

    template<class F, int M >
    struct covector {
      
      typedef math::covector< typename traits<F>::domain, M > domain;
      typedef math::covector< typename traits<F>::range, M > range;
      
      typedef core::const_vector<F> data_type;
      data_type data;
      
      covector(const data_type& data) 
	: data(data) { 
	assert( ((M == -1) && data.size()) || ((M != -1) && (M == int(data.size()) )));

	res.resize( data.size() );
      }
     
      mutable range res;

      const range& operator()(const domain& x) const {
	assert( +data.size() == x.cols() );

	x.each([&](natural i) {
	    res(i) = data(i)(x(i));
	  });

	return res;
      }
      

      struct push : base< covector< typename traits<F>::push, M > > {
	push(const covector& of, const domain& at) 
	  : push::base( push::base::data_type::map( [&](natural i) {
		return d( ref(of.data(i)) )(at(i));
	      }, of.data.size() )) { 
	  assert( of.data.size() == at.cols() );
	}
      };

      struct pull : base< vector< typename traits<F>::pull, M > > {
	pull(const covector& of, const domain& at) 
	  : pull::base( pull::base::data_type::map( [&](natural i) {
		return dT( ref(of.data(i)) )(at(i));
	      }, of.data.size() )) { 
	  assert( of.data.size() == at.cols() );
	}
      };

    };

  }
}




#endif
