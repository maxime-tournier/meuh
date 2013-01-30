#ifndef MATH_MINRES_H
#define MATH_MINRES_H

#include <math/vec.h>
#include <math/iter.h>

#include <math/lanczos.h>

#include <core/macro/derp.h>

#include <script/meuh.h>
#include <core/debug.h>


namespace math {
  
  namespace impl {
    
    // as seen in Choi 2006
    struct minres {
      real beta;

      vec v_prev, v, v_next;
      
      vec d_next, d, d_prev;

      vec r;			// residual
      real phi;			// residual norm
      real tau;
      
      real delta_1;
      real gamma_min; 		// minimum non-zero eigenvalue

      real norm; 		// matrix norm
      real cond;		// condition number

      real c, s;

      real eps;

      natural k;		// iteration 

      // r = b - Ax
      void init(const vec& r, math::real threshold = 0);
      
      static void sym_ortho(const real& a, const real& b,
			    real& c, real& s, real& r);

      template<class Matrix>
      void step(vec& x, const Matrix& A, math::real sigma = 0) {

	if( !phi ) return;
	
	real alpha;
	real beta_prev = beta;
	lanczos::result_type res{alpha, beta, v_next};
	lanczos::step( A, v, v_prev, beta, sigma, res );
	  
	real delta_2 = c * delta_1  +  s * alpha;
	real gamma_1 = s * delta_1  -  c * alpha;

	real eps_next = s * beta;
	real delta_1_next = -c * beta;
	  
	real gamma_2;
	sym_ortho(gamma_1, beta, c, s, gamma_2);
	
	tau = c * phi;
	phi = s * phi;
	
	r = (s * s) * r - (phi * c) * v_next;
  
	norm = (k == 1) ? std::sqrt( alpha * alpha  +  beta * beta ) 
	  : std::max(norm, std::sqrt( alpha * alpha  +  beta * beta  +  beta_prev * beta_prev));
	  
	if( gamma_2 ) {
	  
	  d_next = (v  -  delta_2 * d  -  eps * d_prev ) / gamma_2;
	  x += tau * d_next;
	  
	  gamma_min = (k == 1)? gamma_2 : std::min(gamma_min, gamma_2);
	  assert( gamma_min );
	  
	  cond = norm / gamma_min;
	  
	  d_prev = std::move(d);
	  d = std::move( d_next );
	  
	  v_prev = std::move(v);
	  v = std::move(v_next);
	  
	  eps = eps_next;
	  delta_1 = delta_1_next;
	  
	} else {
	  // core::log( DERP );
	}
	   
	++k;
      }


    };

      
  }


  
  template<class Matrix>
  struct minimum_residual {

    const Matrix matrix;
    
    vec solve(const vec& b, const iter& it, vec x = vec(), const real sigma = 0) const {
      const natural n = b.rows();
      
      impl::minres data;
      
      if( x.empty() ) {
	x = vec::Zero(n);
	data.init(b, it.epsilon);
      } else {
	assert( x.rows() == +n );
	data.init(b - matrix(x), it.epsilon);
      }
      
      it.go( [&] {
	  return (data.step(x, matrix, sigma), data.phi);
	});
      
      // macro_debug("minres sanity check:", "phi:", data.phi, "actual:", (b - matrix(x)).norm());

      return x;
    }
    
  };


  template<class Matrix>
  minimum_residual<Matrix> minres(const Matrix& matrix) { return {matrix}; }
  
}


#endif
