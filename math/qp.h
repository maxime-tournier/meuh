#ifndef MATH_QP_H
#define MATH_QP_H

#include <math/vec.h>
#include <math/iter.h>

#include <math/active_set.h>
#include <math/minres.h>
#include <math/cg.h>



namespace math {

  // a simple active-set minres-based qp solver. only matrix-vector
  // products need to be provided.


  // the @bilateral first constraints in A are bilateral
  
  // TODO add fancy optimal step strategy
  // TODO add a more precise bilateral mask ?
  template<class MatrixQ, class MatrixA, class MatrixAT>
  struct quadratic_program {
    
    const MatrixQ Q;
    
    const MatrixA A;
    const MatrixAT AT;

    quadratic_program(const MatrixQ& Q,
		      const MatrixA& A,
		      const MatrixAT& AT,
		      natural bilateral = 0)
      : Q(Q),
	A(A),
	AT(AT),
	bilateral(bilateral),
	eps(0),
	slack(0) {

    }
      

    natural bilateral;
    real eps;
    vec* slack;
   


    vec solve(const vec& c, const vec& b, const iter& it, vec warm = vec() ) const {
      
      const natural m = c.size();
      const natural n = b.size();
      
      assert(warm.empty() || (warm.size() == m) );
      
      // active set
      active_set active( n, bilateral );
      
      // kkt functor
      vec storage = vec::Zero(m + n);
      vec ATlambda, Qv, Av, v, lambda_mask;
      
      auto kkt = [&](const vec& x) -> const vec& {
	
	if( n ) active.filter(lambda_mask,  x.tail( n ) );
	v = x.head( m );
	
#pragma omp parallel sections
	{
#pragma omp section
	  Qv = Q( v.full() );
#pragma omp section
	  if( n ) ATlambda = AT( lambda_mask.full()  );
#pragma omp section
	  if( n ) active.filter(Av,  A(v.full()) );
	}
	
	storage.head(m) = Qv;
	if(n) storage.head(m) -= ATlambda;
	
	if(n) storage.tail(n) = -Av;
	
	// damping
	if( this->eps && n ) storage.tail( n ) -= this->eps * lambda_mask;
	
	return storage;
      };
      
      // inner/outer iterations
      math::iter outer = it;
      math::iter inner;
      inner.bound = 1 + std::log(m + n);
      
      // inner.bound = outer.bound;
      
      // dense vectors
      vec sol = storage;
      vec rhs = storage;


      // warm start: primal + active set
      if(!warm.empty()) { 
	sol.head( m ) = warm;
      }
      
      warm = sol.head(m);
      
      vec primal, dual;
      
      if(n) {
	primal = A(warm.full()) - b;
	dual = sol.tail(n);
      
	active.update(primal, dual);
      }
      
      rhs.head(m) = -c;
      if( n ) rhs.tail(n) = -active.filter(b);
      
      // minres solver
      impl::minres minres;
      

      // start minres
      natural k = 0;
      
      storage = rhs - kkt(sol);
      minres.init( storage );

      // best residual
      vec best = sol;
      real min = minres.phi;
      
      // outer iteration
      outer.go([&]() -> real {
	  
	  // inner iteration: @sub minres steps
	  inner.go([&]() -> real  {
	      minres.step(sol, kkt);
	      return minres.phi;
	    });


	  // use warm as temporary to avoid alloc
	  warm = sol.head(m);

	  // update active sets
	  if( n ) {
	    primal = A( warm.full() ) - b;
	    dual = sol.tail(n);
	  }
	  
	  // core::log(dual.transpose());
	   
	  // is update needed ?
	  if( n && active.update(primal, dual) ) {
	    active.filter( sol.tail(n), dual );
	    active.filter( rhs.tail(n), -b );
	    
	    storage = rhs - kkt(sol);
	    minres.init( storage );
	  }

	  if( minres.phi < min ) {
	    best = sol;
	    min = minres.phi;
	  }

	  ++k;
	  return minres.phi;
	});
      
      if(n && slack) *slack = best.tail(n);
      
      return best.head(m);
      
    }
 
    


    
     vec stable(const vec& c, const vec& b, const iter& it, real eps = 0, vec warm = vec(), vec* slack = 0 ) const {
      
      const natural m = c.size();
      const natural n = b.size();
      
      assert(warm.empty() || (warm.size() == m) );
      
      // active set
      active_set active( n, bilateral );
      
      // kkt functor
      vec storage = vec::Zero(m + 3 * n);
      vec ATlambda, Qv, Av, v, lambda_mask;
      
      auto kkt = [&](const vec& x) -> const vec& {
	
	auto v = x.head(m);
	auto w = x.segment(m, n);
	auto lambda = x.segment(m + n, n);	
	auto mu = x.tail(n);
	
	storage.head(m) = Q(v) - AT(lambda);
	storage.segment(m, n) = lambda - active.filter(mu);
	storage.segment(m + n, n) = - A(v) + w;
	storage.tail(n) = -active.filter(w);
	
	// damping
	if( eps ) storage.tail( n ) -= eps *  storage.tail( n );
	
	return storage;
      };

      // inner/outer iterations
      math::iter outer = it;
      math::iter inner;
      inner.bound = 1 + std::log(m + 3 * n);
      // inner.bound = outer.bound;
      
      // dense vectors
      vec sol = storage;
      vec rhs = storage;

      // warm start: primal + active set
      if(!warm.empty()) { 
	sol.head( m ) = warm;
      }
      
      warm = sol.head(m);
      
      vec primal, dual;
      primal = A(warm) - b;
      dual = sol.tail(n);
      
      active.update(primal, dual);
      
      rhs << -c, vec::Zero(n), -b, vec::Zero(n);
      
      // minres solver
      impl::minres minres;
     
      // start minres
      natural k = 0;
      
      storage = rhs - kkt(sol);
      minres.init( storage );
      
      // outer iteration
      outer.go([&] () -> real  {
	  
	  // inner iteration: @sub minres steps
	  inner.go([&]() -> real {
	      minres.step(sol, kkt);
	      return minres.phi;
	    });


	  // use warm as temporary to avoid alloc
	  warm = sol.head(m);

	  // update active sets
	  primal = A( warm ) - b;
	  // primal = sol.segment(m, n);
	  
	  dual = sol.tail(n);
	  
	  // core::log(dual.transpose());
	   
	  // is update needed ?
	  if( active.update(primal, dual) ) {
	    active.filter( sol.tail(n), dual );
	    // active.filter( rhs.tail(n), -b );
	    
	    storage = rhs - kkt(sol);
	    minres.init( storage );
	  }

	  ++k;
	  return minres.phi;
	});
      
      if(slack) *slack = sol.segment(m + n, n);

      return sol.head(m);

     }




    
    // old stuff
    
    struct data_type {
      const vec c;
      const vec b;

      const natural m;
      const natural n;

      data_type(const vec& c, const vec& b) 
	: c(c),
	  b(b),
	  m(c.rows()),
	  n(b.rows()),
	  sol( vec::Zero( dim() ) ),
	  mask( vec::Ones( n ) ) {
	
      }

      vec sol;
      vec mask;
      
      natural dim() const { return m + n; }

      auto x() -> decltype( sol.head(m) ) {
	return sol.head(m);
      }

      auto lambda() -> decltype( sol.tail(n) ) {
	return sol.tail(n);
      }
      
      void rhs(vec& out) {
	out.resize( dim() );
	
	out.head(m) = -c;
	out.tail(n) = b.cwiseProduct( mask );
      }
      
      // w = mask * ( b - Ax )
      bool update(const vec& w) {
	bool restart = false;

	auto lambda = this->lambda();
	
	mask.parallel([&](natural i) {
	    
	    // constraint i is violated, should be active
	    if( (w(i) >= 0) && !mask(i) ) {
	      mask(i) = 1.0;
#pragma omp atomic 
	      restart |= 1;
	    } 
	      
	    // lagrange multiplier is attractive, constraint should
	    // be inactive
	    else if( (lambda(i) > 0) && mask(i) ) {
	      mask(i) = 0;
	      lambda(i) = 0;
#pragma omp atomic 
	      restart |= 1;
	    }
	      
	  });
	
	return restart;
      }      

    };



    // TODO warm for lambdas?
    vec solve_old(const vec& c, const vec& b, const iter& it, vec warm = vec(), real eps = 0) const {
    

      data_type data{c, b};
      
      // warm start
      assert( warm.empty() || (warm.rows() == data.m) );
      if( !warm.empty() ) data.x() = warm;
      
      // kkt right hand side
      vec rhs;
      data.rhs( rhs );
      
      // temporaries
      vec work = vec::Zero( data.dim() );
      vec Ax, tmp, w;
      
      bool Ax_ready = false;

      // (symmetric!) kkt matrix
      auto kkt = [&](const vec& y) -> const vec& {
	auto x = y.head( data.m );
	auto lambda = y.tail( data.n);
	
#pragma omp parallel sections
	{
#pragma omp section
	  {
	    tmp = lambda.cwiseProduct( data.mask );
	    work.segment(0, data.m).noalias() = Q(x) + AT( tmp );
	  } 
#pragma omp section
	  {
	    if( !Ax_ready ) Ax = A(x);
	    work.segment(data.m, data.n).noalias() = Ax.cwiseProduct( data.mask );

	    if( eps ) work.segment(data.m, data.n) -= eps * lambda.cwiseProduct(data.mask);
	  }
	}
	return work;

      };

      // minres sub-iterations. magic !
      natural sub = std::ceil( std::log( data.dim() ));
      sub = std::max<int>(sub, 1);
      
      vec residual = rhs - kkt( data.sol );

      // minres data structure
      impl::minres minres;
      minres.init( residual );
      
      natural k = 0;
      
      it.go( [&]() -> real  {
	  
	  // partially solve kkt with minres
	  for(natural i = 0; i < sub; ++i) {
	    minres.step(data.sol, kkt);
	  }
	  
	  Ax = A( data.x() );
	  w = b - Ax;
	  
	  const bool restart = data.update( w );
	  
	  const bool last = (k == (it.bound - 1));
	  // restart kkt solve when mask changed
	  if( restart && !last ) {
	    
	    // update rhs with mask TODO only lagrange multipliers
	    data.rhs( rhs );

	    // restart minres
	    Ax_ready = true;
	    residual = rhs - kkt( data.sol );
	    Ax_ready = false;
	    
	    minres.init( residual );
	  }
	  
	  ++k;
	  return minres.phi;
	});
      
       // core::log("iterations:", k * sub);

      return data.x();
    }

  };


  template<class MatrixQ, class MatrixA, class MatrixAT>
  quadratic_program<MatrixQ, MatrixA, MatrixAT> qp(const MatrixQ& Q,
						   const MatrixA& A,
						   const MatrixAT& AT,
						   natural bilateral = 0) { return {Q, A, AT, bilateral}; }


  // TODO nice wrappers for dense matrices ?

}


#endif
