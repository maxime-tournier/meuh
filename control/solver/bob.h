#ifndef BOB_H
#define BOB_H

#include <phys/system/actuator.h>
#include <phys/system/impulses.h>
#include <phys/system/velocities.h>

#include <phys/solver/numbering.h>
#include <phys/solver/dense.h>

#include <phys/solver/friction.h>
#include <phys/solver/brutal.h>

#include <math/algo/least_squares.h>
#include <math/algo/sor.h>
#include <math/svd.h>

#include <core/macro/debug.h>

#include <phys/exception.h>


namespace phys {

  namespace controller {

    
    namespace factor {

    

      template<class Key>
      struct chunk {
	const typename Key::matrix& matrix;
      
	chunk( const typename Key::matrix& matrix) : matrix(matrix) { 
	  if( matrix.empty() ) return;
	
	  solver::number_dimensions(keys, dofs, matrix);
	  dense = solver::dense( keys, dofs, matrix );

	}
      
	solver::numbering<Key> keys;
	solver::numbering<system::dof> dofs;
      
	math::mat dense;
      
      };

      template<class Key>
      struct affine : chunk<Key>{
	
	math::mat responses;
	
	template<class Solver>
	affine( const typename Key::matrix& matrix, 
		const Solver& solver) 
	  : chunk<Key>(matrix) { 
	  if(matrix.empty() ) return;

	  const math::natural m = this->keys.dimension();
	  const math::natural n = this->dofs.dimension();
	  
	  responses.resize( n, m );
	  
	  const math::vec& origin = solver::dense( this->dofs, solver( system::impulses() ) );
	  
	  for(math::natural i = 0; i < m; ++i) {
	    const system::impulses& j_i = solver::sparse(this->dofs, this->dense.row(i).transpose() );
	    responses.col(i) = solver::dense( this->dofs, solver( j_i ) ) - origin;
	  }
	  
	  
	}
	
      };
      
      
      template<class Key>
      struct full : affine<Key>{
	
	math::mat compliance;
	
	template<class Solver>
	full( const typename Key::matrix& matrix, 
	      const Solver& solver) 
	  : affine<Key>(matrix, solver) { 
	  if(matrix.empty() ) return ;
	  
	  compliance = this->dense * this->responses;
	  // compliance.diagonal().cwise() += 0.01;
	}
	
      };
      
    

    }



    // handles both friction and control
    template<class Solver>
    struct bob {
      
      const system::constraint::matrix& constraints;

      const solver::brutal<Solver> fake;
      const solver::friction<Solver> friction;
            
      const factor::full< system::actuator > actuator;

      const factor::chunk< system::constraint > objective;
      const system::constraint::values& obj_val;
      
      const Solver& solver;

      math::mat A, M, M_inv, K, K_inv;
      math::natural rank;

      bob(const system::constraint::matrix& constraints,
	     const system::constraint::values& const_val,
	     const solver::constraint_projection& proj,
	     const system::actuator::matrix& actuators,
	     const system::constraint::matrix& objectives,
	     const system::constraint::values& obj_val,
	     const Solver& solver) 
	: constraints(constraints),
	  fake(constraints, const_val, solver),
	  friction(constraints, const_val, proj, solver),
	  actuator(actuators, fake),
	  objective(objectives),
	  obj_val(obj_val),
	  solver(solver),
	  rank(0)
      { 

	if( !actuator.keys.dimension () ) return;
	// TODO optimize
	math::mat adapted_actuator_responses;
	adapted_actuator_responses.resize(objective.dofs.dimension(), actuator.keys.dimension() );
	
	for(math::natural i = 0; i < actuator.keys.dimension(); ++i) {
	  adapted_actuator_responses.col(i) = solver::dense(objective.dofs, 
							    solver::sparse(actuator.dofs, 
									   actuator.responses.col(i)) );
	}

	A = objective.dense * adapted_actuator_responses;
	M = actuator.compliance;
	
	M_inv = math::svd(M).inverse();

	math::real lambda = 1;
	K = A.transpose() * A + lambda * M_inv;
	
	math::svd svd(K, 1e-1);
	rank = svd.rank();
	
	macro_debug(rank) << std::endl;;
	
	if(rank) {
	  K_inv = svd.inverse();
	
	  if( math::empty(K_inv) )  throw exception(macro_here + " RHAAAA bis");
	}
      }



      
      system::velocities operator()(const system::impulses& f) const {

	system::velocities resp = fake(f);
	
	if( constraints.empty() ) return resp;	// pas de contacts
	if( !rank ) return friction(f); // pas d'actuation
	
	using sparse::operator*; using sparse::operator-; using sparse::operator+;
	
	// solve the least squares problem
	math::vec c = solver::dense(objective.keys, obj_val - objective.matrix * resp );
	math::vec mu = K_inv * A.transpose() * c;
	// mu.setZero();

	

	std::cout << macro_here << core::tab << "mu: " << mu.transpose() << std::endl;
	system::impulses actuation = solver::sparse(  actuator.dofs,
						      actuator.dense.transpose() * mu );
	
	return friction( f + actuation );
      }
      
    };
    

  }

}


#endif
