#ifndef SPARSE_LINEAR_ELEMENT_H
#define SPARSE_LINEAR_ELEMENT_H

#include <math/vec.h>
#include <map>

namespace sparse {
  namespace linear {

    template<class K>
    void element(math::vec& res, const std::map<K, math::vec>& v, K k) {
      assert( consistent(v) );
      assert( res.rows() == int(dim(k)) );

      auto i = v.find( k );
      if( i == v.end() ) { res = math::vec::Zero( dim(k) ); }
      else  {  
	res = i->second;
      }
    }

    
    template<class K>
    void element(math::vec& res, std::map<K, math::vec>&& v, K k) {
      assert( consistent(v) );
      assert( res.rows() == int(dim(k)) );
      
      auto i = v.find( k );
      if( i == v.end() ) { res = math::vec::Zero( dim(k) ); }
      else  {  
	res = std::move(i->second);
      }
    }
  
  }
}
#endif
