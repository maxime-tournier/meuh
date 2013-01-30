#ifndef MATH_BILEVEL_H
#define MATH_BILEVEL_H

#include <math/vec.h>
#include <math/iter.h>

#include <math/active_set.h>
#include <math/minres.h>

namespace math {

  template<class MatrixQ, 
	   // class MatrixM,
	   class MatrixD, class MatrixDT,
	   class MatrixA, class MatrixAT>
  struct bilevel_quadratic_program {
    
    // min 0.5 * xQx + cx 
    // st. x in argmin 0.5 xMx -fx
    // st. Ax >= b
    
    // m x m
    const MatrixQ Q;

    // k x m
    const MatrixD D;
    const MatrixDT DT;
    
    // n x k
    const MatrixA A;
    const MatrixAT AT;
    
    math::iter iter;
    
    natural bilateral;
    real eps;
    vec* slack;
    
    bilevel_quadratic_program(const MatrixQ& Q,
			      const MatrixD& D,
			      const MatrixDT& DT,
			      const MatrixA& A,
			      const MatrixAT& AT,
			      natural bilateral = 0)
      : Q(Q),
	D(D),
	DT(DT),
	A(A),
	AT(AT),
	bilateral( bilateral ),
	eps(0),
	slack(0) {
      
      assert( eps >= 0 );
      assert( eps < 1 );
  
    }


    vec test(const vec& c, const vec& f, const vec& b, vec warm = vec() ) const {
    
       // dofs
      const natural m = c.size();

      // kinematic dofs
      const natural k = f.size();
      
      // constraints
      const natural n = b.size();

      // total dof size (including lambda)
      const natural ds = m + n;
      
      // total constraint size
      const natural cs = k + n;
      
      // total size 
      const natural s = ds + cs;
      
      // active set
      active_set active( n, bilateral );
      
      // extend A matrices to include actuation variables
      auto AA = [&](const vec& x) -> vec {
	assert( x.size() == m );
	return active.filter( A(x.head(k)) );
      };
      
      // same for transpose
      auto AAT = [&](const vec& x) -> vec {
	assert( x.size() == n );
	
	vec res = vec::Zero(m);
	res.head(k) = AT( active.filter(x) );
	return res;
      };


      auto kkt = [&](const vec& x) {

	vec res = vec::Zero(s);

	res << 
	Q(x.head(m)) + DT(x.segment(ds, k)) + AAT(x.tail(n)),
	-active.filter( A(x.segment(ds, k)) ),
	D(x.head(m)) - AT( active.filter( x.segment(m, n))),
	active.filter( A(x.head(m) ))
	;

	return res;
      };
      

      vec rhs = vec::Zero(s);
      vec primal, dual;
      
      // TODO init active set

      rhs << 
	-c, vec::Zero(n), f, active.filter(b);
      
      // inner/outer iterations
      math::iter outer = iter;
      math::iter inner;
      inner.bound = 1 + std::log(s);

      // minres solver
      impl::minres minres;
     
      // start minres
      vec res = vec::Zero(s);
      
      minres.init( rhs - kkt(res) );
      
      vec best = res;
      real min = minres.phi;

      // outer iteration
      outer.go([&]() -> real {
	  
	  // inner iteration: @sub minres steps
	  inner.go([&]() -> real  {
	      minres.step(res, kkt);
	      return minres.phi;
	    });

	  primal = A(res.head(m)) - b;
	  dual = res.segment(m, n);
	  
	  if( active.update(primal, dual) ) {
	    res.segment(m, n) = active.filter( res.segment(m, n));
	    res.tail(n) = active.filter( res.tail(n) );
	    
	    rhs.tail(n) = active.filter(b);

	    minres.init( rhs - kkt(res) );
	  }

	  if( minres.phi < min ) {
	    best = res;
	    min = minres.phi;
	  }
	  
	  return minres.phi;
	});

      if(slack) *slack = res.segment(m, n);
      
      return res.head(m);
    }
    




    vec solve(const vec& c, const vec& f, const vec& b, vec warm = vec() ) const {
      
      // dofs
      const natural m = c.size();

      // kinematic dofs
      const natural k = f.size();
      
      // constraints
      const natural n = b.size();
      
      const natural ds = m + 2 * n;
      const natural cs = k + 2 * n;

      // wee
      const natural s = ds + cs;
      
      // inactive constraints: positive z
      vec mask = vec::Zero(n);
      
      auto v = [&](const vec& x) { return x.head(k); };
      // auto mu = [&](const vec& x) { return x.segment(k, m - k); };
      auto q = [&](const vec& x) { return x.head(m); };
      
      auto lambda = [&](const vec& x) { return x.segment(m, n); };
      auto z = [&](const vec& x) { return x.segment(m + n, n); };
      
      // auto y = [&](const vec& x) { return x.tail(k + 2 * n); };
      
      // extend A matrices to include actuation variables
      auto AA = [&](const vec& x) -> vec {
	assert( x.size() == m );
	return A(x.head(k));
      };
      
      // same for transpose
      auto AAT = [&](const vec& x) -> vec {
	assert( x.size() == n );
	
	vec res = vec::Zero(m);
	res.head(k) = AT(x.full());
	return res;
      };

      
      // full constraint matrix
      auto C = [&](const vec& x) -> vec {
	assert( x.size() == ds );
	
	vec res = vec::Zero(cs);
	
	res << 
	D( q(x) ) - AT( lambda(x)),
	AA( q(x)) - lambda(x) + z(x),
	lambda(x) - mask.cwiseProduct(z(x));

	return res;
      };

      auto CT = [&](const vec& x) -> vec {
	assert( x.size() == cs);
	
	vec res = vec::Zero(ds);
	
	res << 
	DT(x.head(k)) + AAT(x.segment(k, n) ),
	-A(x.head(k)) - x.segment(k, n) + x.tail(n),
	x.segment(k, n) - mask.cwiseProduct(x.tail(n));
	
	return res;
      };

      
      auto QQ = [&](const vec& x) {
	vec res = vec::Zero(m + 2 * n);
	
	res.head(m) = Q(x.head(m));

	return res;
      };
      
      
      // dense vectors
      vec res = vec::Zero( s ); 
      vec rhs = res;
      
      // active set update
      auto update = [&] {
	bool changed = false;
	    
	mask.each([&](natural i)  {
	    real old = mask(i);
	    
	    mask(i) = z(res)(i) > 0;
	    
	    if( old != mask(i) ) changed = true;
	  });
	return changed;
      };
      
      
      auto kkt = [&](const vec& x) {
	vec res = vec::Zero(s);
	
	res <<
	QQ(x.head(ds)) - CT(x.tail(cs)),
	-C(x.head(ds));
	
	return res;
      };
      
      // inner/outer iterations
      math::iter outer = iter;
      math::iter inner;
      
      inner.bound = (1 + std::log( s ));
      // inner.bound = outer.bound;
      
      if( !warm.empty() ) {
      	assert( warm.size() == m );
      	res.head(m) = warm;
      }
      
      const auto& cres = res;
      mask = ((A(cres.segment(0, k)) - b).array() >= 0).template cast<real>();
      
      rhs << -c, vec::Zero(2 * n), -f, -b, vec::Zero(n);
      
      // minres solver
      impl::minres minres;
      minres.init( rhs - kkt(res) );
      
      vec best = res;
      real min = minres.phi;
	
      // outer iteration
      outer.go([&]() -> real {
	  
	  inner.go([&]() -> real  {
	      minres.step(res, kkt);
	      return minres.phi;
	    });
	  
	  if( update() ) {
	    minres.init( rhs - kkt(res) );
	  }
	  
	  if( minres.phi < min ) {
	    best = res;
	    min = minres.phi;
	  }
	  
	  return minres.phi;
	});
      
      if( slack ) *slack = lambda(best);//0.5 * (best.segment(m, n).cwiseAbs() - best.segment(m, n));
      
      return v(best);
    }
  };
 
    

  template<class MatrixQ, 
	   class MatrixD, class MatrixDT,
	   // class MatrixM, 
	   class MatrixA, class MatrixAT>
  bilevel_quadratic_program<MatrixQ, 
			    MatrixD, MatrixDT,
			    // MatrixM, 
			    MatrixA, MatrixAT > bilevel(const MatrixQ& Q,
							// const MatrixM& M,
							const MatrixD& D,
							const MatrixDT& DT,
							const MatrixA& A,
							const MatrixAT& AT,
							natural bilateral = 0) { 
    // return {Q, M, A, AT, bilateral}; 
    return {Q, D, DT, A, AT, bilateral}; 
  }
    
}


#endif
