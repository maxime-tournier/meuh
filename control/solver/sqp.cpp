#include "sqp.h"

#include <phys/system.h>

#include <sparse/linear/transpose.h>
#include <sparse/linear/diagonal.h>
#include <sparse/linear.h>

#include <math/algo/iter.h>

#include <core/log.h>
// #include <phys/solver/si.h>

#include <math/mosek.h>
#include <phys/solver/qp.h>

namespace control {
  namespace solver {
    
  
    sqp::sqp(const phys::system& phys,
		 const control::system& control,
		 const phys::dof::velocity& previous_vel,
		 math::real dt,
		 bool friction,
		 math::real laziness) 
      : solver(phys, dt),
	num(control),
	act( solver.mosek->variables().after( num.actuator.dim() )),
	A(control.actuator.matrix),
	comp(solver.mosek->constraints().after(1)),
	v_prev(previous_vel),
	dt(dt),
	friction(friction),
	laziness(laziness)
    {
      using namespace phys::solver::qp;
      using namespace math;
      
      const phys::dof* dof = 0;
      
      if( act.dim() ) {
	dof = control.actuator.matrix.begin()->second.begin()->first;
	
	solver.mosek->add( act );

	const math::vec bounds = num.actuator.dense( control.actuator.bounds );
	
      	// actuator dynamics
      	AT<control::actuator>(*solver.mosek, A.matrix, solver.dynamics, act, solver.num.dofs, -dt);
	
      	// bounds
      	solver.mosek->bounds(act, -bounds, bounds);
		
      	// // metric
	// if( laziness ) {
	//   const math::mat& M = phys.masses.find(dof)->second;
			  
	//   const math::mat Mi = M.inverse();
	//   assert( !Mi.empty() ); 
	//   const math::mat MiAt = Mi * A.dense.transpose(); 
	//   qobj [ act ] [ act ] =  (laziness * dt * dt ) * (A.dense * MiAt);
	// }
	

      }

      // solver.mosek->debug(true);
      if( num.feature.dim() ) {

	// actuation
      	assert( A.dofs.size() == 1 );
      	
	assert( dof );

	// actuated dofs variables
	const mosek::variable var = phys::solver::qp::variable(dof, solver.v, solver.num.dofs);

	// ///////////////////////////////////////////////
	
	// features
      	const phys::solver::chunk<control::feature> F(control.feature.matrix);
      	assert( F.dofs.size() == 1);
		
	qobj[ var ][ var ] = (F.dense.transpose() * F.dense) + epsilon * mat::Identity(var.dim(), var.dim());

	vec c = - (F.dense.transpose() * num.feature.dense( control.feature.values ));
	
	solver.mosek->Q( qobj );
	solver.mosek->c(var, c );
      } else {
	const mosek::variable var = phys::solver::qp::variable(dof, solver.v, solver.num.dofs);
	solver.mosek->Q(var, math::mat::Identity(var.dim(), var.dim()));
      }

      // complementarity
      if( solver.mu.dim() ) solver.mosek->add( comp );
      
    }


    
    phys::dof::velocity sqp::operator()(const phys::dof::momentum& f, actuator::vector& old_act,
					  phys::dof::momentum& old_uni) const {
     
      // dynamic constraints
      solver.mosek->b( solver.dynamics, solver.num.dofs.dense(f), math::mosek::bound::fixed );

      // no unilateral, no control
      if( ! solver.mu.dim() ) return solver.num.dofs.sparse( solver.mosek->solve( solver.v ) );

      // TODO cache this matrix in ctor
      phys::solver::chunk< phys::constraint::unilateral > N( solver.system.constraint.unilateral.matrix );
   
      phys::dof::velocity res = v_prev;
      
      math::real alot = 1e7;
      if( sparse::linear::norm(res) > alot ) res = phys::dof::velocity();
      if( sparse::linear::norm(old_uni) > alot ) old_uni = phys::dof::momentum();
      
      // solver.mosek->Q(act, reflexes * math::mat::Identity(act.dim(), act.dim()));
      // solver.mosek->c(act, -reflexes * A.keys.dense(old_act));
      
      math::vec v;
      math::vec mu = math::vec::Zero( N.keys.dim() );
      
      // core::log()("starting iterations");

      math::vec storage;
      math::natural max_iter = 20;
      math::vec n, Nv;
      math::vec Av;

      phys::constraint::unilateral::vector s;
      
      n = N.keys.dense( solver.system.constraint.unilateral.values );

      // old_uni.clear();
      // core::log("sqp start iterations");
      for(math::natural i = 0; i < max_iter; ++i) {

	using sparse::linear::operator-;

	// friction vars
	if( friction ) s = solver.system.constraint.rhs(res);

	// update n
	n = N.keys.dense( solver.system.constraint.unilateral.values - s);
	
	solver.mosek->b(solver.unilateral, n, math::mosek::bound::lower );

	v = N.dofs.dense( res );
	Nv = N.dense * v;
	
	math::real epsilon = mu.dot(Nv - n);
	if( i > 0 && epsilon < 1e-14 ) break;
	
	// linearize complement
	const math::real alpha = 0.9; // advance only alpha in constraint linearization

	solver.mosek->A(comp, solver.mu, (Nv - n).transpose() );
	solver.mosek->A(comp, solver.v, solver.num.dofs.dense( old_uni ).transpose() );

	// solver.mosek->b(comp, mu.transpose() * ( (2 - alpha) * Nv - (1 - alpha) * n), math::mosek::bound::upper );
	solver.mosek->b(comp, mu.transpose() * Nv, math::mosek::bound::upper );
	

	if( laziness ) { 
	  using sparse::linear::operator*;
	  Av = dt * num.actuator.dense(A.matrix * res);
	  
	  solver.mosek->Q(act, laziness * (Av * Av.transpose() ));
	}

	try {
	  storage = solver.mosek->solve(solver.mosek->variables());
	  assert( storage.rows() == solver.mosek->variables().dim() );
	}
	catch( const math::mosek::exception& e) {
	  if( res.empty() ) throw e;

	  core::log("got error after", i, "iterations, returning last.",
		      "complementarity:", mu.dot(Nv - n));
	  
	  return res;
	}
	
	res = solver.num.dofs.sparse( storage.segment( solver.v.start, solver.v.dim() ) );
	mu = storage.segment(solver.mu.start, solver.mu.dim());
	
	old_uni = N.dofs.sparse(N.dense.transpose() * mu);

	core::log("complementarity:", epsilon );
	// core::log("\tsqp s norm:", sparse::linear::norm(s), "complementarity:", epsilon);
	
      }
      
      if( !storage.empty() ) { 
	old_act = A.keys.sparse( storage.segment(act.start, act.dim()));
      }
      return res;
    };


  }
}
