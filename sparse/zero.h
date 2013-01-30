#ifndef SPARSE_ZERO_H
#define SPARSE_ZERO_H

#include <math/vec.h>
#include <math/matrix.h>

namespace sparse {

  // zero
  struct zero { 
    bool operator==(const zero& ) const { return true; }
  };

  template<class A>
  bool operator==(const A& , const zero& ) {
    return false;
  }
  
  template<class A>
  bool operator !=( const A& a, const zero& ) {
    return !(a == zero());
  }

  // TODO put this in a .cpp
  namespace { 
    inline bool operator==(const math::vec& vec, const zero& ) {
      return vec.squaredNorm() < 1e-14;
    }

    inline bool operator==(const math::mat& mat, const zero&) {
      return mat.squaredNorm() < 1e-14;
    }

  }    

}

#endif
