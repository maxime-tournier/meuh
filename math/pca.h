#ifndef MATH_PCA_H
#define MATH_PCA_H

#include <math/types.h>

#include <math/real.h>
#include <math/matrix.h>
#include <math/vec.h>

#include <math/exception.h>

namespace math {

  class pca {
    
    vec mean_;
    mat basis_;
    mat coords_ ;
    vec dev_;
    natural rank_;

  public:

    struct exception {
      struct zero : math::exception { };
      struct full : math::exception { };
    };

    pca();
    pca(mat samples, real eps = math::epsilon);
    pca& compute(mat samples, real eps = math::epsilon, mat metric = mat() );
    
    const mat& basis() const;
    const vec& dev() const;
    const vec& mean() const;
    const mat& coords() const;
    
    bool zero() const;
    bool full() const;
    natural rank() const;

    vec cumul() const;
    
    natural dim() const;
    natural reduced(real) const;
    
    static natural reduced(const vec&, real);
    static vec cumul(const vec&);
    
    // linear subspace in which samples lie: first V vectors
    mat space() const throw( exception::zero );

    // orthogonal to the previous space: last V vectors
    mat orth() const throw( exception::full );
  };

}



#endif
