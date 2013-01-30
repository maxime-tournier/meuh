#ifndef MATH_FUNC_VECTOR_H
#define MATH_FUNC_VECTOR_H

#include <math/vector.h>

#include <math/func.h>
#include <math/func/push.h>
#include <math/func/pull.h>
#include <math/func/ref.h>

#include <math/func/ref.h>

#include <core/const_vector.h>

namespace math {
  namespace func {
    
    template<class F, int M >
    struct vector {
      
      typedef math::vector< typename traits<F>::domain, M > domain;
      typedef math::vector< typename traits<F>::range, M > range;
      
      typedef core::const_vector<F> data_type;
      data_type data;
      
      vector(const data_type& data) 
	: data(data) { 
	assert( ((M == -1) && data.size()) || ((M != -1) && (M == int(data.size()) )));

	res.resize(data.size());
      }
      
      mutable range res;

      const range& operator()(const domain& x) const {
	assert( +data.size() == x.rows() );
	
	x.each([&](natural i) {
	    res(i) = data(i)(x(i));
	  });
	
	return res;
      }
      

      struct push : base< vector< typename traits<F>::push, M > > {
	push(const vector& of, const domain& at) 
	  : push::base( push::base::data_type::map( [&](natural i) {
		return d( ref(of.data(i)) )(at(i));
	      }, of.data.size() )) { 
	  assert( of.data.size() == at.rows() );
	}
      };

      struct pull : base< covector< typename traits<F>::pull, M > > {
	pull(const vector& of, const domain& at) 
	  : pull::base( pull::base::data_type::map( [&](natural i) {
		return dT( ref(of.data(i)) )(at(i));
	      }, of.data.size() )) { 
	  assert( of.data.size() == at.rows() );
	}
      };

    };

    template<class F>
    vector<F> vec(const core::const_vector<F>& data) { return {data}; }

    template<class F>
    vector<F> vec(math::natural i) { return { core::const_vector<F>{i} }; }
    
    
  }
}


#endif
