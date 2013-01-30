#ifndef CONTROLLER_BALANCE_H
#define CONTROLLER_BALANCE_H

#include <phys/system/actuator.h>
#include <phys/system/impulses.h>
#include <phys/system/velocities.h>

#include <phys/solver/numbering.h>
#include <phys/solver/dense.h>

#include <math/algo/least_squares.h>
#include <phys/exception.h>

namespace phys {
  
  namespace controller {

    template<class Solver>
    struct balance {
      
      struct actuator_type {
	const system::actuator::matrix& matrix;
	
	actuator_type( const system::actuator::matrix& matrix) : matrix(matrix) { 
	  if( matrix.empty() ) return;
	  
	  solver::number_dimensions(keys, dofs, matrix);
	  dense = solver::dense( keys, dofs, matrix );
	}

	solver::numbering<system::actuator> keys;
	solver::numbering<system::dof> dofs;
	
	math::mat dense;
      } actuator;


      struct constraint_type {
	const system::constraint::matrix& matrix;
	const system::constraint::values& values;
	
	constraint_type(const system::constraint::matrix& matrix,
		     const system::constraint::values& values )
	  : matrix(matrix), values(values) {
	  if( matrix.empty() ) return;
	  
	  solver::number_dimensions(keys, dofs, matrix);
	  dense = solver::dense( keys, dofs, matrix );
	  
	}
		   

	solver::numbering<system::constraint> keys;
	solver::numbering<system::dof> dofs;
	
	math::mat dense;
      } constraint;

      const Solver& solver;

      math::mat compliance;
      std::unique_ptr<math::algo::least_squares::solve> ls;

      balance( const system::actuator::matrix& actuators,
	       const system::constraint::matrix& constraints,
	       const system::constraint::values& values,
	       const Solver& solver) 
	: actuator(actuators),
	  constraint(constraints, values),
	  solver(solver)
	
      {
	
	const math::natural m = constraint.keys.dimension(), n = actuator.keys.dimension();
	
	assert( !math::nan(actuator.dense) );

	if( m && n  ) {
	  compliance.resize(m, n);
	  
	  for( math::natural i = 0; i < n; ++i) {
	    const system::impulses& a_i = solver::sparse( actuator.dofs, actuator.dense.row(i).transpose() );
	    
	    using sparse::operator*;
	    compliance.col(i) = constraint.dense * solver::dense( constraint.dofs, solver( a_i ) );
	  }
	  assert( ! math::nan( compliance ) && "DURRRRRRRR !" );

	  ls.reset( new math::algo::least_squares::damped( compliance, 1e-1 ) );
	}
      }


      system::velocities operator()(const system::impulses& f) const {

	const system::velocities& resp = solver(f);

	if( actuator.matrix.empty() || constraint.matrix.empty() ) return resp;
	
	const math::natural m = constraint.keys.dimension();
	
	using sparse::operator*; using sparse::operator-;
	const math::vec& rhs = solver::dense( constraint.keys, constraint.values - constraint.matrix * resp );
	
	// const math::vec& lambda = meuh::math::svd(compliance).tikhonov_inverse(1e-1) * rhs;
	const math::vec& lambda = (*ls)( rhs, math::algo::stop().it(m) );
	if( math::nan(lambda) ) throw exception(macro_here + " DURRR" );
	
	// std::cout << HERE << " lambda: " << lambda.norm() << std::endl;
	using sparse::operator+;
	return solver( f + solver::sparse(actuator.dofs, actuator.dense.transpose() * lambda));
      };
      
    };

  }
}



#endif
