#ifndef PHYS_CONTROLLER_TAISTE_H
#define PHYS_CONTROLLER_TAISTE_H

#include <phys/system/actuator.h>
#include <phys/system/impulses.h>
#include <phys/system/velocities.h>

#include <phys/solver/numbering.h>
#include <phys/solver/dense.h>

#include <math/algo/least_squares.h>
#include <math/algo/sor.h>
#include <math/algo/svd.h>

#include <core/macro/here.h>

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
	  
	  for(math::natural i = 0; i < m; ++i) { // TODO range ? (upto)
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
	}
	
      };
      
    }






    // handles both friction and control
    template<class Solver>
    struct taiste {
      
      factor::full<system::constraint> constraint;
      const system::constraint::values& const_val;
      
      math::mat big;
      
      struct projector {
	typedef std::map<math::natural, system::constraint*> constraints_type;
	constraints_type constraints;
	math::natural dimension;

	const solver::constraint_projection& proj;

	projector(const solver::numbering<system::constraint>& n,
		  const solver::constraint_projection& proj) 
	  : dimension(n.dimension()), 
	    proj(proj) {
	  solver::numbering<system::constraint>::const_offset_iterator o, oe;
	  for(std::tie(o, oe) = n.offsets(); o != oe; ++o) {
	    constraints[ o->second ] = o->first;
	  }
	}
	

	math::real clamp(math::real x) const { 
	  math::real abs = std::abs(x);
	  math::real eps = 10;	// CLAMP
	  
	  if(abs > eps) {
	    return x / abs * eps;
	  } else {
	    return x;
	  }

	}
	
	math::real operator()(const math::vec& data, math::natural i) const {
	  if( i >= dimension ) return clamp( data(i) );
	  
	  // 1 trouver la contrainte
	  constraints_type::const_iterator it = constraints.upper_bound(i);
	  assert( it != constraints.begin() && "bad index" );
	    
	  --it;
	  system::constraint* c = it->second;
	    
	  // 2 projeter
	  const math::natural off = it->first; 
	  math::vec chunk = data.segment(off, c->dimension);
	  math::vec p = core::find(proj, c)->econd( chunk ); // TODO find hint ?
	 
	  assert( (i - off) < c->dimension && "bad index" );

	  // 3 renvoyer la bonne valeur
	  return p(i - off);
	}


      };


      projector project;
      
      
      factor::affine<system::actuator> actuator;
      factor::chunk<system::constraint> objective;
      const system::constraint::values& obj_val;

      const Solver& solver;
     

      math::mat A, B, C, D, E, F, G, M_inv, U, V;
      math::natural rank;
      

      math::mat a() const {
	return constraint.compliance;
      }

      math::mat b() const {
	
	math::mat adapted_actuator_responses(int(constraint.dofs.dimension()),
					     int(actuator.keys.dimension() ));
	for(math::natural i = 0; i < actuator.keys.dimension(); ++i) {
	  adapted_actuator_responses.col(i) = solver::dense(constraint.dofs, 
							    solver::sparse(actuator.dofs, 
									   actuator.responses.col(i)) );
	}
	
	return constraint.dense * adapted_actuator_responses;
	
      }
      
      math::mat c() const {

	math::mat adapted_constraint_responses(int(objective.dofs.dimension()), 
					       int(constraint.keys.dimension()));
	
	for(math::natural i = 0; i < constraint.keys.dimension(); ++i) {
	  adapted_constraint_responses.col(i) = solver::dense(objective.dofs, 
							      solver::sparse(constraint.dofs, 
									     constraint.responses.col(i)) );
	}
	

	return objective.dense * adapted_constraint_responses;
	
      }

      math::mat d() const {
	
	math::mat adapted_actuator_responses(int(objective.dofs.dimension()),
					     int(actuator.keys.dimension() ));
	for(math::natural i = 0; i < actuator.keys.dimension(); ++i) {
	  adapted_actuator_responses.col(i) = solver::dense(objective.dofs, 
							    solver::sparse(actuator.dofs, 
									   actuator.responses.col(i)) );
	}

	return objective.dense * adapted_actuator_responses;
      }

      
      taiste(const system::constraint::matrix& constraints,
	     const system::constraint::values& const_val,
	     const solver::constraint_projection& proj,
	     const system::actuator::matrix& actuators,
	     const system::constraint::matrix& objectives,
	     const system::constraint::values& obj_val,
	     const Solver& solver) 
	: constraint( constraints, solver  ),
	  const_val(const_val),
	  project(constraint.keys, proj),
	  actuator(actuators, solver),
	  objective(objectives),
	  obj_val(obj_val),
	  solver(solver)
      {  
	math::natural p = constraint.keys.dimension();
	math::natural q = actuator.keys.dimension();
	math::natural r = objective.keys.dimension();
	
	rank = 0;

	// pas de contacts
	if( !p ) return;
	
	
	// contacts block
	A = a(); assert( A.rows() == p && A.cols() == p );

	if(!q || !r ) return;	

	B = b();  assert( B.rows() == p && B.cols() == q );
	C = c();  assert( C.rows() == r && C.cols() == p );
	D = d(); assert( D.rows() == r && D.cols() == q );
	

	math::mat A_inv = math::algo::svd(A, 1e-30).inverse();

	E = C * A_inv;  assert( E.rows() == r && E.cols() == p);
	F = D - E * B;  assert( F.rows() == r && F.cols() == q);

	const factor::full<system::actuator> fact(actuators, solver);
	
	M_inv = // math::algo::svd(fact.compliance, 1e-30).inverse();
	  fact.compliance.inverse();
	std::cout << macro_here << core::tab << M_inv.rows() << " " << M_inv.cols() << std::endl;
	
	const math::algo::svd svd(F, 0.1);

	rank = svd.rank();
	if( ! rank) return; 

	std::cout << macro_here << core::tab << "rank: " << rank << core::tab << "eigen: " << svd.eigen().transpose() << std::endl; 
	U = svd.im();
	V = svd.kerT();

	G = svd.eigen().start(rank).asDiagonal() * V.transpose();

	big.resize(p + rank, p + rank);
	
	big << A, B * M_inv * G.transpose() ,
	  math::mat::Zero(rank, p), G * M_inv * G.transpose();
	
	
	// math::real eps = 1;
	// math::algo::svd svd(F, eps);

	// rank = svd.rank();

	// math::algo::svd bob(D, eps);

	// std::cout << HERE << "   rank =  " << rank 
	// 	  << " eigen = " << svd.eigen().transpose() 
	// 	  << " obj/act rank = " << bob.rank()
	// 	  << " obj/act eigen = " << bob.eigen().transpose() 
	// 	  << std::endl;
	
	// if( rank ) {
	//   G = svd.tikhonov(eps);
	  
	//   big.resize(p + q, p + q);
	//   big << 
	//     A, B ,
	//     math::mat::Zero( q, p ), math::mat::Identity(q, q);
	  
	//   assert(! math::nan(big) );
	// }
      }



      
      system::velocities operator()(const system::impulses& f) const {

	const system::velocities& resp = solver(f);

	math::natural p = constraint.keys.dimension();
	math::natural q = actuator.keys.dimension();
	math::natural r = objective.keys.dimension();
	
	if( !p ) return resp;	// pas de contacts


	using sparse::operator*; using sparse::operator-; using sparse::operator+;

	// only contacts
	if( !rank ) { 	// big is not set here !
	  math::vec rhs = solver::dense(constraint.keys, const_val - constraint.matrix * resp );
	  assert( !math::nan(rhs) );

	  math::vec lambda = math::algo::sor(A, 1)(rhs, project, math::algo::stopper().iterations(100) );
	  assert( !math::nan(lambda) );

	  return this->solver( f + solver::sparse(constraint.dofs, constraint.dense.transpose() * lambda ) );
	  
	}
	
	// std::cout << HERE << "  values: constraint " << solver::dense(constraint.keys, const_val).squaredNorm()
	// 	  << "  objective " << solver::dense(objective.keys, obj_val).squaredNorm()
	// 	  << std::endl;

	using sparse::operator*; using sparse::operator-;
	math::vec e = solver::dense( constraint.keys, const_val - constraint.matrix * resp );
	assert( !math::nan(e) );

	math::vec g = solver::dense( objective.keys, obj_val - objective.matrix * resp );
	assert( !math::nan(g) );

	math::vec rhs;
	rhs.resize(p + rank );
	
	rhs << e, U.transpose() * (g - E * e);
	assert( !math::nan(rhs) );

	
	math::vec solution = math::algo::sor(big, 1)(rhs, project, math::algo::stopper().iterations(2000) );
	assert( !math::nan(solution) );

	math::vec lambda = solution.start( p ); 	  assert( !math::nan(lambda) );
	math::vec mu = solution.end( rank ); 	  assert( !math::nan(mu) );
	
	const system::impulses actuation =  solver::sparse(actuator.dofs, actuator.dense.transpose() *  M_inv * G.transpose() * mu );
	
	math::vec michel = solver::dense( constraint.keys, constraint.matrix * solver( actuation ) );
	math::vec true_lambda = math::algo::sor(A, 1)(e - michel, project, math::algo::stopper().iterations(1000) );
	

	std::cout << macro_here
		  << "\tmu " << mu.transpose() << std::endl;


	using sparse::operator+;
	return solver( f 
		       + solver::sparse(constraint.dofs, constraint.dense.transpose() * true_lambda )
		       + actuation );
	
      }

    };
    

  }

}


#endif
