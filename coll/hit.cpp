#include "hit.h"

#include <core/error.h>


namespace coll {

  math::real hit::sign(const primitive* p) const {
    if( p == first ) return -1.0;
    if( p == second ) return 1.0;
    
    throw core::error("primitive not involved in hit");
  }
  
  // relative velocity is v2 - v1 (as given by normal)
  math::vec3 hit::dir(const primitive* p) const {
    return sign(p) * normal;
  }
  

}
