#ifndef PHYS_SOLVER_CHOLESKY_H
#define PHYS_SOLVER_CHOLESKY_H

#include <sparse/diff.h>
#include <sparse/sum.h>
#include <sparse/prod.h>
#include <sparse/dense.h>

#include <phys/constraint/bilateral.h>

#include <phys/solver/compliance.h>
#include <phys/solver/task.h>
#include <phys/error.h>

#include <Eigen/Cholesky>
#include <math/gpu.h>

#include <core/log.h>

namespace phys {
  namespace solver {

    // TODO should be called dense ?
    template<class Solver>
    struct cholesky {

      const Solver solver;
      
      const solver::compliance<constraint::bilateral> K;
      
      const math::real epsilon;

      typedef Eigen::LDLT< Eigen::MatrixXd > inv_type;
      //  typedef math::gpu inv_type;
      inv_type inv;
      
      cholesky(const Solver& solver,
	       const constraint::bilateral::matrix& matrix,
	       math::real epsilon = 0)
	: solver(solver),
	  K(solver, matrix, epsilon),
	  epsilon(epsilon)
      {
	if(!K.matrix.empty() ) inv.compute( K.dense ); // inv.compute(K.dense, Eigen::ComputeThinU | Eigen::ComputeThinV);
      }

      cholesky(Solver&& s,
	       const constraint::bilateral::matrix& matrix,
	       math::real epsilon = 0)
	: solver(std::move(s) ),
	  K(solver, matrix, epsilon),
	  epsilon(epsilon)
      {
	if(!K.matrix.empty() ) inv.compute( K.dense ); // inv.compute(K.dense, Eigen::ComputeThinU | Eigen::ComputeThinV);
      }


      cholesky(const Solver& s,
	       const system& sys)
	: solver(s),
	  K(s, sys.constraint.bilateral.matrix, sys.constraint.bilateral.damping),
	  epsilon(0)
      {
	if(!K.matrix.empty() ) inv.compute( K.dense );
      }

      cholesky(const cholesky& ) = default;
      cholesky(cholesky&& other) 
	: solver( std::move( other.solver )),
	  K( std::move( other.K ) ),
	  epsilon( other.epsilon),
	  inv( other.inv ) {
	// TODO move inv
      }
      

      void solve( const solver::task& task) const {
	solver.solve(task);
	
	if( K.matrix.empty() ) return;
	
	using sparse::operator-;
	using sparse::operator*;

	const math::vec rhs = sparse::dense(K.matrix.rows(), 
					    task.bilateral - K.matrix * task.velocity);
	
	const math::vec dlambda = inv.solve( rhs );
	const constraint::bilateral::vector lambda = sparse::dense( K.matrix.rows(), dlambda );
	
	if( task.lambda ) task.update(*task.lambda, lambda);

	// we need to resolve to update subsolver lambdas
	solver::task sub = task;
	sub.momentum += sparse::transp( K.matrix ) * lambda;
	solver.solve( sub );
	
	// using sparse::operator+=;
	// task.velocity += sparse::dense(K.matrix.cols(), K.linear * dlambda); 
      }
      
    };
      

    template<class Solver>
    cholesky< typename std::decay<Solver>::type > make_cholesky(Solver&& solver, const constraint::bilateral::matrix& matrix,
								 math::real eps = 0) {
      return { std::forward<Solver>(solver), matrix, eps};
    }


    template<class Solver>
    cholesky<Solver> make_cholesky(const Solver& solver, const phys::system& sys) {
      return { solver, sys };
    }


  }
}



#endif
