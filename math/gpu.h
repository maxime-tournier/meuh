#ifndef MATH_GPU_H
#define MATH_GPU_H

#include <math/types.h>
#include <math/mat.h>

namespace math {
  
  struct gpu {

    Eigen::MatrixXf A;
    
    gpu(const mat& A);
    gpu();
    
    void compute(const mat& A);
    vec solve(const vec& b) const;



  };

}


#endif
