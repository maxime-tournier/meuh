#include "panda.h"

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
    
  
    panda::panda(const phys::system& phys,
		 const control::system& control,
		 const phys::dof::velocity& previous_vel,
		 math::real dt,
		 bool relax,
		 math::real laziness,
		 math::real reflexes) 
      : solver(phys, dt),
	final(phys, dt),
	num(control),
	act( solver.mosek->variables().after( num.actuator.dim() )),
	A(control.actuator.matrix),
	dt(dt),
	relax(relax),
	laziness(laziness),
	reflexes(reflexes)
    {
      using namespace phys::solver::qp;
      using namespace math;
      
      // solver.mosek->debug(true);

      // default setup
      setup = [&](const phys::dof::momentum& f ){
	// dynamic constraints
	solver.mosek->b( solver.dynamics, solver.num.dofs.dense(f), math::mosek::bound::fixed );

	using sparse::linear::operator-;
	using sparse::linear::operator*;
	phys::solver::qp::c(*solver.mosek, relax ? (c  - f) : c
			    , solver.v, solver.num.dofs);
	  
      };


      if( act.dim() && num.feature.dim() ) {
	
	// actuation
      	solver.mosek->add( act );
	
      	
      	assert( A.dofs.size() == 1 );
      	const phys::dof* dof = control.actuator.matrix.begin()->second.begin()->first;

      	const math::vec bounds = num.actuator.dense( control.actuator.bounds );
	
      	// actuator dynamics
      	AT<control::actuator>(*solver.mosek, A.matrix, solver.dynamics, act, solver.num.dofs, -dt);
	
      	// bounds
      	solver.mosek->bounds(act, -bounds, bounds);
		
      	// metric
      	const math::mat Mi = phys.mass.find(dof)->second.inverse();
	const math::mat MiAt = Mi * A.dense.transpose();
	K =  A.dense * MiAt;

	// AM^-1A^T
      	if( relax ) solver.mosek->Q(act, ( dt * dt ) * K);

	// off-diagonal: actuation work v^TA^T mu
	math::natural start = solver.v.start + solver.num.dofs.off(dof);
	math::mosek::variable actuated_dof_vars( start , start + dof->dim() ); // actuated dofs

	if( relax ) solver.mosek->Q(act, actuated_dof_vars, (-dt * laziness) * A.dense );
	
	// adds a term to c
	setup = [&, setup, dof, MiAt](const phys::dof::momentum& f ){
	  setup(f);
	  if( relax ) solver.mosek->c(act,  (dt) * (MiAt.transpose() * f.find(dof)->second));
	};
	
	// ///////////////////////////////////////////////
	
	// features
      	const phys::solver::chunk<control::feature> F(control.feature.matrix);
      	assert( F.dofs.size() == 1);
	
	// const phys::dof* dof = control.feature.matrix.begin()->second.begin()->first; 
	
      	phys::dof::matrix Q;
	
	math::real weight = 1;

      	Q[ dof ] = weight * (F.dense.transpose() * F.dense);
      	c[ dof ] = - weight * (F.dense.transpose() * num.feature.dense( control.feature.values ));
	
	if( relax ) add_Q(*solver.mosek, Q, solver.v, solver.num.dofs);
	else phys::solver::qp::Q(*solver.mosek, Q, solver.v, solver.num.dofs);
	
	// smooth yo !
	using sparse::linear::operator*;
	const vec prev = num.actuator.dense( A.matrix * previous_vel );
	
	if( false &&  (prev.norm() > 1e-6)) {
	  
	  math::mosek::constraint taiste = solver.mosek->constraints().after(1);
	  solver.mosek->add( taiste );
	  
	  solver.mosek->A(taiste, act, prev.transpose() );
	  solver.mosek->b(taiste, vec1::Zero(), mosek::bound::upper );
	  
	  // if( relax ) solver.mosek->add_Q(act, laziness * (zob * zob.transpose()));
	  // else solver.mosek->Q(act,  laziness * (zob * zob.transpose()) );
	}

	if(false && solver.unilateral.dim() ) {
	  // pseudo-complementarity
	  // math::mosek::constraint taiste = solver.mosek->constraints().after(1);
	  // solver.mosek->add( taiste );
	  
	  using sparse::linear::operator*;
	  using sparse::linear::operator-;
	  
	  phys::constraint::unilateral::vector bob = (phys.constraint.unilateral.matrix * previous_vel) 
	    - phys.constraint.unilateral.values;
	  
	  math::vec dense_bob = solver.num.unilateral.dense(bob);
	  
	  solver.mosek->Q(solver.mu,  dense_bob * dense_bob.transpose());
	  
	  // solver.mosek->A(taiste, solver.mu, dense_bob.transpose() );
	  // solver.mosek->b(taiste, vec1::Zero(), mosek::bound::upper );
	}

      }


    

    }


    


    // TODO move stuff in a simple physics solver
    phys::dof::velocity panda::operator()(const phys::dof::momentum& f, actuator::vector& old_act) const {
     
      setup(f);
      
      // hurrr
      using namespace math;
      vec storage = solver.mosek->solve(solver.mosek->variables());

      vec v = storage.segment(solver.v.start, solver.v.dim());
      vec a = storage.segment(act.start, act.dim());
      vec mu = storage.segment(solver.mu.start, solver.mu.dim());
            
      // core::log()("actuation:", a.transpose());

      phys::dof::velocity sv = solver.num.dofs.sparse(v);
      phys::constraint::unilateral::vector smu = solver.num.unilateral.sparse(mu);
      
      using sparse::linear::operator*;
      using sparse::linear::operator-;

      phys::constraint::unilateral::vector n = solver.system.constraint.unilateral.matrix * sv 
	- solver.system.constraint.unilateral.values;
      math::real comp = sparse::linear::dot(n, smu);
      
      core::log()("complementarity:", comp);
      // core::log()("act energy:", a.dot( K * a) );
      
      using sparse::linear::operator+;
      return final(f + A.dofs.sparse( A.dense.transpose() * (dt * a)) );
      
      // return solver.num.dofs.sparse( v );
    };


  }
}
