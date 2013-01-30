#ifndef MATH_LIE_SAMPLES_H
#define MATH_LIE_SAMPLES_H

#include <core/unused.h>
#include <math/lie.h>

#include <math/matrix.h>
#include <math/vector.h>

namespace math {
  namespace lie {

    // this is for converting tangent values to plain data back and
    // forth when doing statistical analysis for instance
    template<class G>
    struct samples {
      const group<G> lie;
      
      typedef typename group<G>::algebra algebra;
      typedef typename euclid::space<algebra>::field field;
            
      samples(const group<G>& lie) : lie(lie) { }

      matrix<field> coords(const vector<algebra>& data) const {
	const natural m = data.rows();
	const natural n = lie.alg().dim();

	matrix<field> res;
	res.resize(m, n);
	
	data.each([&](natural i) {
	    math::euclid::get(res.row(i), data(i));
	  });
	
	return res;
      }

      vector<algebra> elems(const matrix<field>& data) const {
	const natural m = data.rows();
	const natural n = lie.alg().dim();
	
	core::unused{n};
	assert( data.cols() == int(n) );
	
	vector<algebra> res;
	res.resize(m);
	
	auto zero = lie.alg().zero();
	res.each([&](natural i) {
	    res(i) = zero;	// for allocs
	    math::euclid::set(res(i), data.row(i) );
	  });
	
	return res;
      }
      
    };

  }
}


#endif
