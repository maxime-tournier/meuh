#include "cpg.h"


namespace math {
  namespace algo {

    namespace proj {
      
      box::box(const vec& lower, const vec& upper) 
	: lower(lower), upper(upper) {
	assert( lower.rows() == upper.rows() );
	assert( ! (lower.array() > upper.array()).any() );
      }

      vec box::operator()(const vec& x) const {
	assert(x.rows() == lower.rows());
	return lower.array().max( upper.array().min(x.array()) );
      }

      vec box::tangent(const vec& x, const vec& v) const {
	assert(x.rows() == lower.rows());
	assert(x.rows() == v.rows());
	
	vec res = v;
	res.each([&](natural i ) {
	    if( x(i) == this->lower(i) ) res(i) = std::max(v(i), 0.0); 
	    else if( x(i) == this->upper(i) ) res(i) = std::min(v(i), 0.0); 
	  });
	
	return res;
      }
      
      // TODO the rest

    }

  }

}

