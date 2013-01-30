#ifndef MATH_LIE_GEOMETRIC_MEAN_H
#define MATH_LIE_GEOMETRIC_MEAN_H

#include <math/lie.h>
#include <math/vector.h>
#include <math/algo/iter.h>

#include <math/euclid/arithmetic_mean.h>
#include <core/log.h>

#include <math/iter.h>

namespace math {
  namespace lie {
    
    template<class G>
    struct geometric_mean {
      const group<G> lie;      

      // linearize in tangent plane
      vector< typename group<G>::algebra > tangent(const G& g,
						   const vector<G>& samples) const {
	vector< typename group<G>::algebra > res;
	res.resize(samples.rows());
	
	const G g_inv = lie.inv(g);
	
	res.each( [&](natural i) {
	    res(i) = this->lie.log()( lie.prod(g_inv, samples(i)));
	  });
	
	return res;
      }
      

      G operator()(const vector<G>& samples, 
		   const math::iter& iter ) const {
	// core::log()("computing geometric mean...");
	G res = lie.id();
	
	// iterates
	iter.go([&]{
	    // linearizes in tangent, takes euclidean mean
	    const typename group<G>::algebra tangent_mean = 
	      math::euclid::mean(this->lie.alg()) ( this->tangent(res, samples) );
	    
	    res = this->lie.prod(res, this->lie.exp()( tangent_mean )); // advances
	    real eps = this->lie.alg().norm( tangent_mean );
	    // std::cout << eps << std::endl;
	    
	    return eps;
	  });

	return res;
      }
      

    };

    template<class G>
    geometric_mean<G> mean(const group<G>& lie = group<G>()) { return {lie}; }

  }
}



#endif
