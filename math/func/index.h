#ifndef MATH_FUNC_INDEX_H
#define MATH_FUNC_INDEX_H

#include <math/vector.h>

#include <set>
#include <map>

#include <core/stl.h>

namespace math {
  namespace func {
    namespace index {

      template<class U>
      struct select {

	typedef math::vector<U> domain;
	typedef math::vector<U> range;

	const std::set<natural>& indices;
	select(const std::set<natural>& indices) : indices(indices) { }

	range operator()(const domain& x) const {
	  range res;
	  res.resize( indices.size() );
	
	  natural i = 0;
	  core::each(indices, [& ]( natural src ) {
	      res(i) = x(src);
	      ++i;
	    });
	  return res;
	}

	T<range> diff(const T<domain>& dx) const {
	  range at;
	  typename Lie<range>::algebra body;

	  at.resize( indices.size() );
	  body.resize( indices.size() );
	
	  natural i = 0;
	  using namespace core;
	  core::each(indices, [&]( natural src ) {
	      at(i) = dx.at()(src);
	      body(i) = dx.body()(src);
	      ++i;
	    });
	  
	  return math::body(at, body);
	}

      };
    


      template<class U>
      struct map {

	typedef math::vector<U> domain;
	typedef math::vector<U> range;
	
	// input -> output
	const std::map<natural, natural>& indices;
	
	// TODO check indices consistency in ctor ?

	range operator()(const domain& x) const {
	  range res;
	  res.resize( indices.size() );
	  
	  core::each(indices, [&]( natural in, natural out) {
	      res(out) = x(in);
	    });
	  return res;
	}
	
	T<range> diff(const T<domain>& dx) const {
	  range at;
	  typename Lie<range>::algebra body;

	  at.resize( indices.size() );
	  body.resize( indices.size() );
	
	  core::each(indices, [ & ]( natural in, natural out) {
			   at(out) = dx.at()(in);
			   body(out) = dx.body()(in);
	    });
	  
	  return math::body(at, body);
	}
	
      };
      
      
      




    }
  }
}



#endif
