#ifndef MATH_CG_H
#define MATH_CG_H

#include <math/vec.h>
#include <math/iter.h>

#include <core/log.h>
#include <core/macro/derp.h>

namespace math {

  namespace impl {

    struct cg {
      vec p;			// descent direction
      vec r;			// residual
      vec Ap;			// A(p)
      
      real phi2;		// residual squared norm
      real phi;			// residual norm
      
      natural k;		// iteration

      void init(const vec& rr);
      
      template<class Matrix>
      bool step(vec& x, const Matrix& A) {
	Ap = A(p);
	const real pAp = p.dot(Ap);
	
	// fail
	if( !pAp ) { 
	  core::log(DERP); 
	  return false;
	}
	
	const real alpha = phi2 / pAp;
	
	x += alpha * p;
	r -= alpha * Ap;

	const real old = phi2;
	
	phi2 = r.squaredNorm();
	phi = std::sqrt( phi2 );
	const real mu = phi2 / old;
	
	p = r + mu * p;
	++k;
	
	return true;
      }
    };
  }

  
  template<class Matrix>
  struct conjugate_gradient {
    
    const Matrix matrix;
    
    vec solve(const vec& b, const iter& it, vec x = vec() ) const {
      const natural n = b.rows();

      impl::cg data;
      
      if(x.empty()) { 
	x = vec::Zero( n );
	data.init( b );
      } else {
	assert( x.rows() == +n );
	data.init( b - matrix(x) );
      }

      it.go([&] {
	  return data.step(x, matrix) ? data.phi : 0;
	});
      
      if( x.empty() ) return vec::Zero(n);
      
      return x;
    }
    

  };

  template<class Matrix>
  conjugate_gradient< typename  std::decay<Matrix>::type > cg(Matrix&& A) { return { std::forward<Matrix>(A) }; }
  
}



#endif
