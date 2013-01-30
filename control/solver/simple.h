#ifndef CONTROL_SOLVER_SIMPLE_H
#define CONTROL_SOLVER_SIMPLE_H


#include <phys/actuator.h>
#include <phys/solver/affine.h>

#include <math/svd.h>
#include <sparse/linear.h>
#include <sparse/linear/dense.h>
#include <sparse/linear/numbering.h>



namespace math {
  struct svd;
}

namespace control {

  namespace solver {

    namespace cache { 
      struct constraints {
	const phys::constraint::matrix& matrix;
	const phys::constraint::values& values;

	sparse::linear::numbering<phys::constraint> keys;
	sparse::linear::numbering<phys::dof> dofs;
	
	math::mat dense;
	
	constraints(const phys::constraint::matrix& matrix,
		    const phys::constraint::values& values )
	  : matrix(matrix), values(values) {
	  if( matrix.empty() ) return;
	  
	  sparse::linear::number(keys, dofs, matrix);
	  dense = sparse::linear::dense( keys, dofs, matrix );
	  
	}
	
      };


      struct actuators {
	const phys::actuator::matrix& matrix;

	sparse::linear::numbering<phys::actuator> keys;
	sparse::linear::numbering<phys::dof> dofs;

	math::mat dense;

	actuators(const phys::actuator::matrix& matrix)
	  : matrix(matrix) {
	  if( matrix.empty() ) return;
	  
	  sparse::linear::number(keys, dofs, matrix);
	  dense = sparse::linear::dense( keys, dofs, matrix );
	  
	}

      };


    }
      
    template<class Solver>
    struct simple {

      const phys::solver::affine<phys::actuator> actuator;
      const phys::actuator::vector& weights;

      const cache::constraints constraint;
      

      const Solver solver;
      
      std::unique_ptr<math::svd> inv;
      math::mat Q;
      // give actuators, objectives, values + underlying solver
      // solver is assumed to give affine response
      simple( const phys::actuator::matrix& actuators,
	      const phys::actuator::vector& weights,
	      const phys::constraint::matrix& constraints,
	      const phys::constraint::values& values,
	      const Solver& solver) 
	: actuator(actuators, solver),
	  weights(weights),
	  constraint(constraints, values),
	  solver(solver)
      {
	if( actuators.empty() || constraints.empty() ) return;
	
	using namespace math;
	
	const natural m = constraint.dense.rows();
	const natural n = actuator.dense.cols();
	
	// // we need actuator compliance and actuator values
	// mat M = actuator.compliance();

	// weights
	mat W = actuator.keys.dense( weights ).asDiagonal();
	
	std::cout << "WEIGHTS : " << W.diagonal().transpose() << std::endl;
	// values for actuator responses
	Q.resize(constraint.keys.dim(), actuator.keys.dim());

	for(natural i = 0, n = Q.cols(); i < n; ++i) {
	  Q.col(i) = constraint.dense * 
	    constraint.dofs.dense( actuator.dofs.sparse( actuator.linear.col(i)));
	}

	math::real lambda = 0.001;
	// assert( !nan(M) );
	assert( !nan(Q) );
	assert( !nan(W) );
	
	inv.reset( new math::svd( Q.transpose() * Q + lambda * W) );
      }
      
      dof::velocity operator()(const dof::momentum& f) const {
	const dof::velocity resp = solver(f);
	if(!inv) return std::move(resp);
	
	using namespace math;
	vec rhs = constraint.keys.dense(constraint.values); 
	rhs.noalias() -= constraint.dense * constraint.dofs.dense(resp);
	
	const vec lambda = inv->solve(Q.transpose() * rhs);
	assert(!nan(lambda) );

	std::cout << lambda.norm() << std::endl;

	using sparse::linear::operator+;
	return solver( f + actuator.dofs.sparse(actuator.dense.transpose() * lambda));
      }

    };
    


  }
}


#endif
