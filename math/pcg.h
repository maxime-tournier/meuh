#ifndef MATH_PCG_H
#define MATH_PCG_H

#include <math/cg.h>

#include <core/log.h>
#include <core/macro/derp.h>

namespace math {

  namespace impl {

    struct pcg : cg {

      vec z;
      real rz;
      
      template<class Prec>
      void init(const vec& rr, const Prec& prec) {
	cg::init(rr);

	z = prec(rr);

	p = z;
	rz = r.dot(z);
      }
      
      template<class Matrix, class Prec>
      bool step(vec& x, const Matrix& A, const Prec& prec) {

	Ap = A(p);
	const real pAp = p.dot(Ap);
	
	// fail
	if( !pAp ) { 
	  core::log(DERP); 
	  return false;
	}
	
	const real alpha = rz / pAp;
	
	x += alpha * p;
	r -= alpha * Ap;
	z = prec( r );
	
	const real old = rz;
	phi2 = r.squaredNorm();
	phi = std::sqrt( phi2 );
	rz = r.dot( z );
	
	const real mu = rz / old;

	p = z + mu * p;
	++k;
	return true;
      }
    };
  }

  
  template<class Matrix, class Prec>
  struct preconditioned_conjugate_gradient {
    
    const Matrix matrix;
    const Prec prec;
    
    vec solve(const vec& b, const iter& it, vec x = vec() ) const {
      const natural n = b.rows();
      
      impl::pcg data;
      
      if(x.empty()) { 
	x = vec::Zero( n );
	data.init( b, prec );
      } else {
	assert( x.rows() == +n );
	data.init( b - matrix(x), prec );
      }

      it.go([&] {
	  bool ok = data.step(x, matrix, prec);
	  return ok ? data.phi : 0;
	});
      
      if( x.empty() ) return vec::Zero(n);
      
      return x;
    }
    

  };

  template<class Matrix, class Prec>
  preconditioned_conjugate_gradient< typename std::decay<Matrix>::type,
				     typename std::decay<Prec>::type> pcg(Matrix&& A,
									  Prec&& prec) { 
    return { std::forward<Matrix>(A),
	std::forward<Prec>(prec)}; 
  }
  

}


#endif
  
