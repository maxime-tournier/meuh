#ifndef SPARSE_LINEAR_DIMENSION_H
#define SPARSE_LINEAR_DIMENSION_H

#include <math/types.h>
#include <core/error.h>

#include <map>
#include <core/stl.h>
#include <sparse/linear/traits.h>


namespace sparse {
  namespace linear {
  
    template<class K>
    math::natural dim(K k) {
      return traits<K>::dimension(k);
    };
    
    template<class K>
    math::natural dim(const std::map<K, math::vec>& vec) {
      math::natural res = 0;
      core::each(vec, [&](K k, const math::vec& ) {
	  res += dim(k);
	});

      return res;
    }

  
  }
}

#endif
