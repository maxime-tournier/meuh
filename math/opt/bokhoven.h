
#ifndef MATH_OPT_BOKHOVEN_H
#define MATH_OPT_BOKHOVEN_H

#include <math/vec.h>
#include <math/minres.h>

#include <core/log.h>

namespace math {
  namespace opt {
    
    namespace qp {




      // a simple QP solver based on Van Bokhoven 'modulus algorithm'
      // formulation and minres/active set
      template<class MatrixQ, class MatrixA, class MatrixAT>
      struct bokhoven {

	MatrixQ Q;
	MatrixA A;
	MatrixAT AT;

	natural bilateral;

	math::iter iter;

	real eps;
	vec* slack;
	
	bokhoven(const MatrixQ& Q,
		 const MatrixA& A,
		 const MatrixAT& AT,
		 natural bilateral = 0)
	  : Q(Q),
	    A(A),
	    AT(AT),
	    bilateral(bilateral),
	    iter(),
	    eps(0), 
	    slack(0)
	{
	  assert(eps >= 0);
	  assert(eps < 1);
	}


	
	// natural bilateral;
	vec solve(const vec& c, const vec& b, const math::iter& it, vec warm = vec() ) const {
	  
	  natural m = c.rows();
	  natural n = b.rows();

	  vec res = vec::Zero(m + n);

	  // active constraints: negative z (= slack)
	  vec mask = vec::Ones(n);
	  
	  // active set update
	  auto update = [&]() -> natural {
	    natural changed = 0;
	    
	    mask.each([&](natural i)  {
		if(i < bilateral) return;

		real old = mask(i);
		
		mask(i) = res.tail(n)(i) < 0 ? 1 : 0;
		
		if( old != mask(i) ) ++changed;
	      });
	    return changed;
	  };


	  // warm start
	  if( !warm.empty() ) res.head(m) = warm;

	  const auto& cres = res;

	  // initial active set
	  mask = ((A(cres.segment(0, m)) - b).array() < 0).template cast< real > ();

	  // res.tail(n) = (vec::Ones(n) - mask).cwiseProduct( A(res.head(m)) - b );
	  // update();
	  
	  if( bilateral ) mask.head( bilateral ).setOnes();
	  
	  vec storage; storage.resize(m + n);

	  real nu = 1.0 / ( 1.0 - eps );
	  
	  vec v, z, Av, tmp; 

	  // TODO paralolize
	  auto kkt = [&](const math::vec& x) -> const vec& {
	    
	    v = x.head(m);
	    z = x.tail(n);
	    Av = A(v.full());

	    tmp = z - Av;

	    storage.head(m) = Q(v.full()) + nu * AT(tmp.full());
	    storage.tail(n) = nu * Av - nu * z + mask.cwiseProduct( z );
	    
	    return storage;
	  };

	  vec rhs; rhs.resize( m + n );
	  rhs << -c - nu * AT(b.full()), nu * b;
	
	  math::impl::minres minres;
	  minres.init(rhs - kkt(res));
	  
	  math::iter inner;
	  inner.bound = 1 + std::log(m + n);
	  // inner.bound = it.bound;
	  
	  natural k = 0;
	  
	  it.go([&]() -> real  {
	      
	      inner.go([&]() -> real  {
		  minres.step(res, kkt);
		  return minres.phi;
		});
	      
	      if( update() ) {
		
	      	// core::log("k:", k, res.tail(n).transpose());
	      	// core::log("k:", k, mask.transpose());
	      	// std::cout << k << " ";
	      	minres.init(rhs - kkt(res) );
	      }
	      
	      ++k;
	      return minres.phi;
	    });
	  
	  if( slack ) *slack = 0.5 * (res.tail(n).cwiseAbs() - res.tail(n));
	  return res.head(m);
	}
	
	
      };


      template<class MatrixQ, class MatrixA, class MatrixAT>
      bokhoven<MatrixQ, MatrixA, MatrixAT> make_bokhoven(const MatrixQ& Q,
							 const MatrixA& A,
							 const MatrixAT& AT) {
	return {Q, A, AT};
      }
            
    }
  }
}


#endif
