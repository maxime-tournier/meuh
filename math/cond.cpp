#include "cond.h"

#include <math/vec.h>
#include <math/mat.h>
#include <Eigen/Eigenvalues>

namespace math {


  real cond( const vec& eigen) {
    
    real min = 1e42;
    real max = 0;
    
    eigen.each([&](natural i) {
	real ei = std::abs( eigen(i) );

	if( ei < min ) min = ei;
	if( ei > max ) max = ei;

      });
    assert( min );
    return max / min;
  }
  
  
  real cond_symm(const mat& M) {
    return cond( M.selfadjointView<Eigen::Lower>().eigenvalues() );
  }

}
