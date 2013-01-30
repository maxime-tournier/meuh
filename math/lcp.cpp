#include "lcp.h"

#include <math/error.h>
#include <math/qp.h>

namespace math {

  lcp::lcp(const mat& M) : M(M) { 
    
    if(M.rows() != M.cols()) throw error("need a square matrix");
    
  }

  lcp::lcp(mat&& M) : M(std::move(M)) { 
    
    if(M.rows() != M.cols()) throw error("need a square matrix");
    
  }
  
  natural lcp::dim() const { return M.cols(); }
  
};
