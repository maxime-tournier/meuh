#ifndef SPARSE_LINEAR_NAN_H
#define SPARSE_LINEAR_NAN_H

#include <map>
#include <math/nan.h>
#include <core/stl.h>

namespace sparse {
  namespace linear {
    
    template<class K, class C>
    bool nan(const std::map<K, C>& data ) {
      assert( consistent(data) );
      
      bool res = false;
      core::each(data, [&res](K, const C& c) {
	  res = res || math::nan( c );
	});
      
      return res;
    }

    template<class K1, class K2, class C>
    bool nan(const std::map< K1, std::map<K2, C> >& data ) {
      assert( consistent(data) );
      
      bool res = false;
      core::each(data, [&res](K1, const std::map<K2, C>& c) {
	  res = res || math::nan( c );
	});
      
      return res;
    }
    
  
  }
}

#endif
