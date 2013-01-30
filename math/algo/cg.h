#ifndef MATH_ALGO_CG_H
#define MATH_ALGO_CG_H

#include <math/vec.h>
#include <math/matrix.h>
#include <math/real.h>

#include <math/iter.h>
#include <math/error.h>

#include <core/macro/derp.h>


namespace math {

  namespace algo {

    namespace psd {

      struct matrix {
	const mat& data;

	matrix(const mat& data);
	vec operator()(const vec& x) const;
      };

      struct identity {
	const vec& operator()(const vec& x) const { return x; }
      };
    }

    struct early_stop { const vec solution; };
    
    template<class PSD = psd::matrix,
	     class Prec = psd::identity >
    struct conjugate_gradient {
      
      const PSD psd;
      const Prec prec;
      
      // some oracle for early exit
      std::function< bool ( const vec& ) > oracle;
      
      conjugate_gradient( const PSD& psd, const Prec& prec = Prec() )
	: psd( psd ),
	  prec( prec )
      {
	
      }
      
      vec solve(const vec& b, math::iter iter, vec x = vec()) const {
	const natural dim = b.rows();

	if( x.empty() ) x = vec::Zero(dim );
	if( int(dim) != x.rows() ) throw error( "incoherent dimensions" );
	
	vec r = b - psd(x);
	vec z = prec(r);
	
	vec p = z;
	vec Ap;

	real r2 = r.squaredNorm(); 
	real zr = z.dot(r);

	real eps = 0;
	natural k = 0;

	iter.go( [&] {
	 
	     
	    eps = std::sqrt( r2 / dim );
	    
	    // early stop in case we reached solution already
	    if( eps <= iter.epsilon ) return eps;
	    
	    Ap = this->psd( p );
	    
	    real pAp = p.dot( Ap );
	    
	    if( !safe(pAp) ) {
	      core::log("warning ! CG near-singularity !", k);
	      
	      if(!pAp) {
	      	core::log("hurr ! CG singularity !");
	      	return 0.0;
	      }

	    }
	    
	    const real alpha = zr / pAp; // wiki
	    
	    x.noalias() += alpha * p;
	    r.noalias() -= alpha * Ap;
	    
	    z = prec( r );
	    
	    if( oracle && oracle(x) ) throw early_stop{ x };
	    
	    // real old_r2 = r2; 
	    r2 = r.squaredNorm();
	    
	    real old_zr = zr;
	    zr = z.dot( r );
	    
	    // real beta = r2 / old_r2; // wiki
	    real beta = zr / old_zr;
	    
	    p.noalias() = z + beta * p;
	    
	    ++k;
	    return eps;
	  });
	
	return x;
      };

    };

    // convenience 
    template<class PSD>
    conjugate_gradient<PSD> cg(const PSD& psd) { return {psd}; }

    template<class PSD, class Prec>
    conjugate_gradient<PSD, Prec> cg(const PSD& psd, const Prec& prec) { return {psd, prec}; }
    
  }
}
#endif
