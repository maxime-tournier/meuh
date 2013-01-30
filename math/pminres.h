#ifndef MATH_PMINRES_H
#define MATH_PMINRES_H

#include <math/minres.h>

namespace math {

  namespace impl {

    struct pminres  {

      vec z_prev, z, z_next;
      vec p, q, q_next;

      real beta_prev, beta, beta_next;

      real delta_1;

      vec d_prev, d, d_next;

      real c, s;

      real eps;
      
      natural k;
      real phi;

      vec r;

      template<class Prec>
      void init(const Prec& prec, const vec& rr) {
	natural n = rr.rows();
	auto zero = vec::Zero(n);
	
	r = rr;
	
	z_prev = zero;
	z = r;
	
	q = prec(z);
	
	beta = std::sqrt( z.dot(q) );
	beta_prev = 1.0;

	phi = beta;
	if( !phi ) return;
	
	delta_1 = 0;

	d = zero;
	d_prev = zero;

	c = -1;
	s = 0;

	eps = 0;

	k = 1;
	
      }



      template<class Matrix, class Prec>
      void step(vec& x, const Matrix& A, const Prec& Minv)  {
	if( !phi ) return;
	
	p = A(q);

	const real alpha = q.dot(p) / (beta * beta);

	z_next = (1 / beta) * p - (alpha / beta) * z - (beta / beta_prev) * z_prev;

	q_next = Minv( z_next );

	beta_next = std::sqrt( q_next.dot(z_next) );
	
	const real delta_2 = c * delta_1 + s * alpha;
	const real gamma_1 = s * delta_1 - c * alpha;

	const real eps_next = s * beta_next;
	delta_1 = -c * beta_next;

	real gamma_2;
	minres::sym_ortho(gamma_1, beta_next, c, s, gamma_2);

	const real tau = c * phi;
	// const real psi = phi * std::sqrt( gamma_1 * gamma_1 +  delta_1 * delta_1 );
	phi = s * phi;
	
	if( gamma_2 ) {
	  
	  d_next = (1.0 / gamma_2) * ( q / beta  -  delta_2 * d  - eps * d_prev);

	  x += tau * d_next;
	  
	  // TODO update formula for r and ||r|| !!!!
	  r -= tau * A(d_next);
	  
	  d_prev = d;
	  d = d_next;

	}
	
	z_prev = z;
	z = z_next;

	beta_prev = beta;
	beta = beta_next;
	
	eps = eps_next;

	q = q_next;
	++k;
      }


    };



  }


  template<class Matrix, class Prec>
  struct preconditioned_minimum_residual {
    const Matrix matrix;
    const Prec prec;

    vec solve(const vec& b, const iter& it, vec x = vec() ) const {
      const natural n = b.rows();
      
      impl::pminres data;
      
      if( x.empty() ) {
	x = vec::Zero(n);
	data.init(prec, b);
      } else {
	assert( x.rows() == +n );
	data.init(prec, b - matrix(x));
      }
      
      it.go( [&] {
	  data.step(x, matrix, prec);
	  
	  return data.r.norm(); 
	  return data.phi;
	});
      
      macro_debug("pminres sanity check:", "phi:", data.phi, "actual:", (b - matrix(x)).norm());

      return x;

    }
    
  };
  

  template<class Matrix, class Prec>
  preconditioned_minimum_residual< typename std::decay<Matrix>::type,
				   typename std::decay<Prec>::type >
  pminres(Matrix&& A, Prec&& Minv) {
    return {std::forward<Matrix>(A), std::forward<Prec>(Minv) };	
  }

}



#endif
