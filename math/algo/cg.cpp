#include "cg.h"

namespace math {
  namespace algo {

    namespace psd {
      
      matrix::matrix(const mat& data) : data(data) { 
	if(data.rows() != data.cols() ) throw error("invalid dimensions !");
      }
      
      vec matrix::operator()(const vec& x) const {
	if( x.rows() != data.cols() ) throw error("invalid vector");
	return data * x;
      }
      
    }

  }
}
