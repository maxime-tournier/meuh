#ifndef MATH_ALGO_CPG_H
#define MATH_ALGO_CPG_H

#include <math/vec.h>
#include <math/algo/stop.h>
#include <math/algo/iter.h>

#include <math/psd.h>
#include <math/error.h>

namespace math {
  namespace algo {


    template<class, class> struct cpg;
    
    // conjugate projected gradient: see Renouf, Alart 2005 or Preclik,
    // Iglberger 2009
    template<class PSD, class Proj>
    struct cpg<psd<PSD>, Proj> {
      const psd<PSD> A;
      const Proj proj;
      
      vec solve(const vec& b, stop s = stop(), vec init = vec()) const {
	if( b.rows() != int(A.dim()) ) throw error("invalid rhs");
	if( init.empty() ) init = vec::Zero(b.rows());
	if( b.rows() != init.rows() ) throw error( "incoherent sizes" );
	
	vec x = proj(init);
	
	vec r = b - A(x);
	vec p = r;
	
	vec pr = proj.tangent(x, r);
	// vec pp;

	const natural n = b.rows();
	if( s.iterations > n ) s.iterations = n;
	
	
	algo::iter_eps( s, [&](real& eps) {
	    
	    const vec Ap = this->A(p); // TODO alloc

	    const real pAp = p.dot(Ap);
	    assert( pAp );

	    const real alpha = r.dot(p) / pAp;
	    
	    x = this->proj(x + alpha * p);
	    r.noalias() -= alpha * Ap;
	
	    // projected residual
	    pr = this->proj.tangent(x, r);	    
	    // pp = this->proj.tangent(x, p);	    

	    const real beta = -pr.dot(p) / pAp;
	    p = this->proj.tangent(x, pr + beta * p);
	    // p = pr + beta * pp;
	    if( p.squaredNorm() < math::epsilon ) p = pr;
	    
	    const real pr_squared = pr.squaredNorm(); 
	    eps = std::sqrt( pr_squared );
	   

	  });
	
	return x;
      };
      

    };
    
    
    template<class PSD, class Proj>
    vec cpg_qp(const PSD& Q, const vec& b, const Proj& proj,
	       const algo::stop& s = stop(),
	       vec init = vec() ) {
      return cpg< psd<PSD>, Proj>{ psd<PSD>(Q), proj }.solve(-b, s, init);
    }
    

    
    // projectors
    namespace proj {

      struct positive {

	vec operator()(const vec& ) const;
	vec tangent(const vec&, const vec& ) const;

      };

      struct lower {

	const vec& bound;
	
	vec operator()(const vec& ) const;
	vec tangent(const vec&, const vec& ) const;


      };

      struct upper {
	const vec& bound;
	
	vec operator()(const vec& ) const;
	vec tangent(const vec&, const vec& ) const;
      };



      struct box {

	box(const vec&, const vec& );

	const vec& lower;
	const vec& upper;
	
	vec operator()(const vec& ) const;
	vec tangent(const vec&, const vec& ) const;

      };
      
      struct sym_box {

	const vec& bound;

	vec operator()(const vec& ) const;
	vec tangent(const vec&, const vec& ) const;


      };
      
    }




  }
}


#endif
