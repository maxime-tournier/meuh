#include "psd.h"

#include <math/matrix.h>
#include <math/vec.h>

namespace math {
  

  vec psd< mat >::operator()(const vec& x) const {
    assert( x.rows() == int(dim()) );
    
    return M * x;    
  }

  natural psd<mat>::dim() const { return M.rows(); }

  psd<mat>::psd(const mat& M) : M(M) { assert( M.cols() == M.rows() ); }

}
