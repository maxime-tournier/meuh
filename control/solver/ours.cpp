#include "ours.h"


#include <phys/solver/qp.h>

#include <control/system.h>
#include <phys/system.h>

#include <sparse/linear.h>

namespace control {
  namespace solver {

   
    static math::real weight = 1e8;

    ours::ours(const phys::system& phys,
	       const control::system& control,
	       math::real dt) 
      : phys::solver::qp::direct(phys, dt),
	control(control),
	feature( control.feature.matrix )
    {
      
      if(feature.keys.dim() ) {
	assert( feature.dofs.size() == 1);

	const phys::dof* dof = control.feature.matrix.begin()->second.begin()->first;
	
      	phys::dof::matrix Q;
	
      	Q[ dof ] = dt * feature.dense.transpose() * feature.dense;
      	c[ dof ] = - dt * feature.dense.transpose() * feature.keys.dense( control.feature.values );
	
	phys::solver::qp::add_Q(*mosek, Q, v, num.dofs);

	// wee
	const math::mat& M = system.mass.find(dof)->second;
	const math::mat Mi = M.inverse();
	
	// actuated dof
	const math::mosek::variable var = phys::solver::qp::variable(dof, v, num.dofs);

	const math::mosek::constraint taiste = mosek->constraints().after(6);
	const math::mosek::variable bret = mosek->variables().after(6);
	mosek->add(bret);
	
	mosek->A(taiste, var, Mi.topRows<6>() * Q[dof]);
	mosek->A(taiste, bret, 1/dt * math::mat66::Identity());
	
	mosek->b(taiste, -Mi.topRows<6>() * c[dof].head<6>(), math::mosek::bound::fixed);
	
	// bretelles penalization
	mosek->Q(bret, weight * math::mat66::Identity());
	
	math::natural inner = var.dim() - 6;
	const math::mosek::constraint zob = mosek->constraints().after(inner);
	mosek->add(zob);

	const phys::solver::chunk<control::actuator> A(control.actuator.matrix);
	const math::natural a = A.dense.rows();
	const math::vec bounds = A.keys.dense( control.actuator.bounds );
	
	mosek->A(zob, var, Q[dof].bottomRows(inner));
	mosek->b(zob, -c[dof].tail(inner) - bounds, -c[dof].tail(inner) + bounds);

	// // bret
	// const math::mosek::variable bret = mosek->variables().after(6);
	// mosek->add(bret);
	
	// mosek->A(taiste, var, Mi.topRows<6>() * Q[dof]);
	// mosek->A(taiste, bret, 1/dt * math::mat66::Identity());
	
	// mosek->b(taiste, -Mi.topRows<6>() * c[dof], math::mosek::bound::fixed);
	
	// // bretelles penalization
	// mosek->Q(bret, weight * math::mat66::Identity());
	
	// mosek->debug( true );
	
	// // constraint actuator projection
	// const phys::solver::chunk<control::actuator> A(control.actuator.matrix);
	// const math::natural a = A.dense.rows();

	// const math::mosek::constraint actuation = mosek->constraints().after(a);
	// mosek->add(actuation);
	
	// const math::mosek::variable act = mosek->variables().after(a);
	// mosek->add(act);

	// const math::vec bounds = A.keys.dense( control.actuator.bounds );
	// mosek->A(actuation, var, A.dense);
	// mosek->b(actuation, -bounds, bounds);

	// mosek->A(actuation, act, -dt * math::mat::Identity( a, a ) );
	// mosek->b(actuation, -A.dense * c[dof], math::mosek::bound::fixed);
	
	// core::log( "bounds: ", bounds.transpose());
	// mosek->bounds(act, -bounds, bounds);
	
      }      

    }


    phys::dof::velocity ours::operator()(const phys::dof::momentum& f) const {
      
      phys::dof::vector test;
      
      
      // objective
      using sparse::linear::operator-;
      phys::solver::qp::c(*mosek, c - f - test, v, num.dofs);
      
      return num.dofs.sparse( mosek->solve(v) );
    };


  }
}
