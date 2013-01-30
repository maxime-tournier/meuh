#ifndef PHYS_SOLVER_SCHUR_H
#define PHYS_SOLVER_SCHUR_H

#include <phys/solver/any.h>

namespace phys {
  namespace solver {

    
    namespace schur {

      // give it a phys solver
      template<class Solver>
      struct bilateral {
	const Solver solver;

	typedef phys::constraint::bilateral constraint_type;
	const typename constraint_type::matrix J;
	
	typedef typename constraint_type::vector vec;
	
	mutable vec res, backup;
	mutable dof::velocity v;
	mutable phys::solver::task task;

	const vec damping;
	const vec levmar;
	
	// WARNING FIXME this is only for IDENTITY mass matrix
	vec make_levmar(math::real eps) const {

	  if(! eps ) return {};
	  
	  vec res(J.rows(), math::vec());

	  J.each([&](phys::constraint::bilateral::key r,
		     phys::dof::key ,
		     sparse::const_mat_chunk Jrc) {
		   res(r) += eps * Jrc.rowwise().squaredNorm();
		 });
	  
	  return res;
	};
	

	bilateral(const Solver& solver,
		  const typename constraint_type::matrix& J,
		  const phys::constraint::bilateral::vector& damping = phys::constraint::bilateral::vector(),
		  math::real eps = 0)
	  : solver(solver),
	    J(J),
	    res( J.rows(), math::vec() ),
	    task(v),
	    damping(damping),
	    levmar( make_levmar(eps) )
	{

	}

	// wtf ?
	bilateral(const bilateral& other) 
	  : solver(other.solver),
	    J(other.J),
	    res(J.rows(), math::vec()),
	    task(v),
	    damping(other.damping),
	    levmar(other.levmar)
	{

	};



	const vec& operator()(const vec& lambda) const {
	  
	  sparse::prod(task.momentum, sparse::transp(J), lambda);
	  
	  // we do this in case the lambda given in parameter is 
	  // erased by solve
	  backup = lambda;

	  v.zero();
	  solver.solve( task );
	  
	  sparse::prod(res, J, v);
	  
	  if(!damping.empty() ) res += sparse::hadamard(damping, backup);
	  if(!levmar.empty() ) res += sparse::hadamard(levmar, backup);
	  
	  return res;
	}



	sparse::vec_chunk operator()(const math::vec& lambda) const {
	  res.dense() = lambda;

	  this->operator()(res);

	  return res.dense();
	}
	

	dof::velocity Minv(const dof::momentum& f) const {
	  task.momentum = f;
	  solver.solve( task );
	  return task.velocity;
	}

	vec rhs(const dof::momentum& f, const vec& b ) const {
	  vec tmp(J.rows(), math::vec());
	  
	  using sparse::operator-;
	  tmp = b - J * Minv(f);

	  // filter result according to J
	  // TODO alloc
	  vec res(J.rows(), math::vec());
	  J.rows().each([&](constraint::bilateral::key c,
			    math::natural ) {
			  res(c) = tmp(c);
			});
	  
	  assert(res.keys().dim() == J.rows().dim());

	  return res;
	}

	vec sparse(const math::vec& dense) const {
	  return vec(J.rows(), dense);
	}

	math::vec dense(const vec& x) const {
	  return sparse::dense(J.rows(), x);
	}

	dof::velocity vel(const dof::momentum& f, const vec& lambda) const {
	  return Minv( f + sparse::transp( J ) * lambda );
	}
      };


      template<class Solver>
      bilateral<Solver> make_bilateral(const Solver& solver, const constraint::bilateral::matrix& J) {
	return {solver, J};
      }

      bilateral<phys::solver::simple> make_bilateral(const phys::system& system, math::real eps = 0) {
	return { phys::solver::simple(system), system.constraint.bilateral.matrix, 
	    system.constraint.bilateral.damping, eps};
      }
      
      
      template<class Algo>
      struct iterative {
	
	const Algo algo;
	math::iter iter;
	
	bool warm;

	iterative(const Algo& algo, const math::iter& iter, bool warm = false)
	  : algo(algo),
	    iter(iter),
	    warm(warm) {

	}

	void solve(const solver::task& t) const {
	  auto rhs = algo.matrix.rhs(t.momentum, t.bilateral);
	  
	  math::vec start;
	  if( warm ) {
	    assert(t.lambda);

	    start = algo.matrix.dense(*t.lambda);
	  }

	  // TODO warm start
	  math::vec dense = algo.solve( rhs.dense(), iter, start);
	  
	  // TODO data.res should act on this task to allow nested
	  // solver to update lambdas
	  auto lambda = algo.matrix.sparse(dense);
	  
	  t.velocity = algo.matrix.vel(t.momentum, lambda);
	  
	  // TODO update lambdas
	  if(t.lambda ) {
	    t.update(*t.lambda, lambda);
	  }
	}

      };
      

      template<class Algo>
      iterative<typename std::decay<Algo>::type > make_iterative(Algo&& algo, const math::iter& iter ) {
	return { std::forward<Algo>(algo), iter};
      }
      
    }



    // template<class Solver>
    // struct schur {
    //   const Solver solver;
      
    //   const constraint::bilateral::matrix& J;
    //   const constraint::unilateral::matrix& N;
      
    //   schur(const Solver& solver,
    // 	    const constraint::bilateral::matrix& J,
    // 	    const constraint::unilateral::matrix& N)
    // 	: solver(solver),
	  
    // 	  J(J),
    // 	  N(N) {
	
    //   }
	  
      
    //   void operator()(constraint::bilateral::vector& lambda,
    // 		      constraint::unilateral::vector& mu) const {
    // 	// TODO alloc 
    // 	dof::velocity v;	
    // 	task t(v);
	
    // 	t.momentum = sparse::transp(J) * lambda + sparse::transp(N) * mu;
    // 	solver.solve( t );
	
    // 	lambda = J * v;
    // 	mu = N * v;
    //   }
      
    //   // TODO without alloc ?
    //   math::vec operator()(const math::vec& x) const {
    // 	const math::natural m = J.rows().dim();
    // 	const math::natural n = N.rows().dim();
	
    // 	constraint::bilateral::vector lambda = sparse::dense(J.rows(), x.segment(0, m) );
    // 	constraint::unilateral::vector mu = sparse::dense(N.rows(), x.segment(m, n) );
	
    // 	operator()(lambda, mu);
	
    // 	math::vec res( m + n );
	
    // 	if( m ) res.segment(0, m) = lambda.dense();
    // 	if( n ) res.segment(m, n) = mu.dense();

    // 	return res;
    //   }
      
    // };



  }
}


#endif
