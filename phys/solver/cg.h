#ifndef PHYS_SOLVER_CG_H
#define PHYS_SOLVER_CG_H

#include <sparse/matrix.h>
#include <phys/constraint/bilateral.h>

#include <math/algo/iter.h>

#include <phys/solver/task.h>

#include <sparse/prod.h>
#include <sparse/sum.h>
#include <sparse/diff.h>
#include <sparse/transp.h>
#include <sparse/dot.h>
#include <sparse/scalar.h>
#include <sparse/dense.h>
#include <sparse/norm.h>
#include <sparse/hadamard.h>


#include <math/minres.h>
#include <math/cgne.h>
#include <math/cg.h>
#include <math/pcg.h>

#include <math/algo/cg.h>

#include <core/macro/here.h>


namespace phys {
  namespace solver {

     
    namespace impl { 

      struct identity {

	template<class A>
	const A& operator()(const A& a) const { return a; }
	
      };

    }

    template<class Solver, class Prec = impl::identity>
    struct cg { 
      
      Solver solver;

      constraint::bilateral::matrix matrix;
      constraint::bilateral::vector damping;
      
      Prec prec;

     

      struct config_type {
	math::real precision;
	math::real damping;
	math::natural iterations;
	bool warm;
	
	bool use_minres;

	config_type() 
	  : precision(1e-14),
	    damping(0),
	    iterations(100),
	    warm( false ),
	    use_minres( false )
	{
	}
      };
      
      config_type config;
      
      cg(const Solver& solver,
	 const constraint::bilateral::matrix& matrix,
	 const Prec& prec = Prec())
	: solver(solver),
	  matrix(matrix),
	  prec(prec)
      { 
	script::require("log");
      }

      cg(Solver&& s,
	 const constraint::bilateral::matrix& matrix,
	 Prec&& prec = Prec() )
	: solver(std::move(s)),
	  matrix(matrix),
	  prec(prec)
	  
      { 
	script::require("log");
      }


      cg(const Solver& solver,
	 const system& sys,
	 const Prec& prec = Prec() ) 
	: solver(solver),
	  matrix(sys.constraint.bilateral.matrix),
	  // damping(sys.constraint.bilateral.damping),
	  prec( prec )
      {
	
      }

      cg(const cg& ) = default;
      cg(cg&&) = default;

      typedef constraint::bilateral::vector vec;
      
      struct stop { 
	stop( const vec& lambda) : lambda(lambda) { }
	vec lambda;
      };
      
      // early stop based on constraint forces
      std::function< bool (const dof::velocity& ) > early_stop;
      mutable std::function< dof::velocity( const constraint::bilateral::vector& ) > sub_resp;
      
      vec solve(const vec& b, vec xx = vec() ) const {
	
	// v must be ordered as matrix.rows()
	auto to_sparse = [&](const math::vec& v) -> vec{
	  return {this->matrix.rows(), v };
	};	
	
	dof::velocity data{ matrix.cols(), math::vec{} };
	
	auto JMinvJT = [&] (const vec& lambda) -> vec {
	  
	  // put MinvJTlambda into data
	  solver::task task(data);
	  task.momentum = sparse::transp(this->matrix) * lambda;
	  solver.solve(task);
	  
	  using namespace sparse;

	  auto res = this->matrix * data;

	  if( !this->damping.empty() ) res += hadamard(this->damping, lambda);
	  
	  if( config.damping ) {
	    res += config.damping * lambda;
	  }

	  return res;
	  
	};

	
	math::vec db = sparse::dense(matrix.rows(), b);
	
	math::vec storage;
	auto psd = [&](const math::vec& y) -> const math::vec& {
	  assert( !math::nan(y) );
	  sparse::dense(storage, this->matrix.rows(), JMinvJT( to_sparse(y) ));
	  
	  return storage;
	};

	math::vec prec_storage;
	
	auto preconditioner = [&](const math::vec& lambda) -> const math::vec& {
	  prec_storage = prec( to_sparse(lambda) ).dense();
	  return prec_storage;
	};
	
	
	// const math::algo::stop stop(config.precision, config.iterations);
	math::iter iter(config.iterations, config.precision);
	
	math::vec init;
	if( config.warm ) init = sparse::dense(matrix.rows(), xx );
	else init.setZero( matrix.rows().dim() );
	
	// setup plots
	script::require("plot");
	script::exec("cg = plot.new(); cg.plot['1:2'] = {with = 'lines'}; cg.set.logscale = 'y'");
	
	iter.cb = [&](math::natural k, math::real eps) {
	  script::exec("cg.data[ %% ] = {%%}", k, eps);
	  core::log("cg:", k, eps);
	};
	

	try {	
	  // auto impl = math::cg(psd);  
	  // auto impl = math::minres( psd );
	  // auto impl = math::cgne( psd );
	  
	  // TODO callback spaghetti
	  if(early_stop) {
	    // impl.oracle = [&] (const math::vec& x) -> bool {
	    //   return early_stop( sub_resp( to_sparse(x) ) );
	    // };
	  }
	  
	  math::vec res;
	  if(config.use_minres) {
	     res = math::minres( psd ).solve( db, iter);
	  } else {
	    res = math::cg( psd ).solve( db, iter );
	  }	  
	  
	  if( res.empty() ) res = init;
	  return to_sparse( res );
	}
	catch( const math::algo::early_stop& s) {
	  return sparse::dense(this->matrix.rows(), s.solution );
	}
	
      }
      

      void solve(const solver::task& task) const {
	solver.solve(task);
	if( matrix.empty()) return;

	using sparse::operator*;
	using sparse::operator-;
	using sparse::operator+;


	if( early_stop ) {
	  sub_resp = [&](const constraint::bilateral::vector& lambda) {
	    dof::velocity v;
	    solver::task sub(v);
	    sub.momentum = task.momentum;
	    sub.bilateral = task.bilateral;
	    sub.momentum += sparse::transp(this->matrix) * lambda;
	    solver.solve( sub );

	    return v;
	  };
	  
	}

	// we solve: JMinvJT lambda = rhs
	const vec rhs = task.bilateral - matrix * task.velocity;

	vec lambda;
	if( task.lambda ) lambda = *task.lambda; // warm start
	lambda = solve(rhs, lambda);
	
	// update original lambdas
	if( task.lambda ) task.update(*task.lambda, lambda);
	
	solver::task sub = task;
	sub.momentum += sparse::transp(matrix) * lambda;
	
	solver.solve(sub);
      }


    };

    template<class Solver>
    cg<typename std::decay<Solver>::type> make_cg(Solver&& solver, const constraint::bilateral::matrix& matrix) { 
      return {std::forward<Solver>(solver), matrix}; 
    }

    template<class Solver, class Prec>
    cg<typename std::decay<Solver>::type, 
       typename std::decay<Prec>::type> make_cg(Solver&& solver, const constraint::bilateral::matrix& matrix, Prec&& prec) { 
      return {std::forward<Solver>(solver), matrix, std::forward<Prec>(prec)}; 
    }
    
    


    template<class Solver>
    cg<Solver> make_cg(const Solver& solver, const phys::system& sys) {
      return {solver, sys};
    }

  }
}



#endif
