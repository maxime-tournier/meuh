#ifndef MATH_SVD_H
#define MATH_SVD_H

#include <math/real.h>
#include <math/matrix.h>
#include <math/vec.h>

#include <math/exception.h>

namespace math {
  
  class svd {
   
    mat u_, v_;
    vec singular_;
      
    natural rank_;

  public:
    svd();
    svd(const mat& m, real eps = 0 );
    svd& compute(const mat& m, real eps = 0, 
		 unsigned int options = (Eigen::ComputeThinU | Eigen::ComputeThinV) );

    const mat& u() const;
    const mat& v() const;
      
    const vec& singular() const;
    
    // only for symmetric
    vec eigen() const;

    natural rank() const;
      
    bool zero() const;
    bool full() const;

    struct exception {
      struct zero : math::exception { };
      struct full : math::exception { };
    };
    
    // moore-penrose pseudo-inverse obtained by ignoring kernel
    // eigenvectors
    mat inverse() const throw(exception::zero);

    // voids singular values below threshold 
    mat fix(real eps = 0) const;
    
    // moore-penrose pseudo-inverse solve
    vec solve( const vec& rhs ) const throw(exception::zero);
        
    // tinhonov inverse obtained by damping singular value 
    mat tikhonov( real = math::epsilon ) const;

    // tinhonov inverse solve
    vec tikhonov_solve( const vec& rhs, real = math::epsilon ) const;

    // an orthogonal basis of the image: first U vectors
    mat im() const throw(exception::zero);

    // an orthogonal basis of the kernel: last V vectors
    mat ker() const throw(exception::full);

    // kernel orthogonal complement : first V vectors
    mat ker_orth() const throw( exception::zero );
      

    bool psd() const;
    bool pd(real eps = 0) const;

  };
  
}


#endif
