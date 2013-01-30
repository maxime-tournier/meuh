#include "precise.h"

#include <phys/solver/qp.h>
#include <phys/system.h>

#include <sparse/linear.h>
#include <core/log.h>

namespace phys {
  namespace solver {
    namespace qp {

      precise::precise(const phys::system& system, math::real dt) 
	: direct(system, dt),
	  lambda(mosek->variables().after(num.bilateral.dim())),
	  mu(lambda.after(num.unilateral.dim())),
	  nu(mu.after(num.friction.dim())),
	  
	  dynamics(mosek->constraints().after( num.dofs.dim() ) )// ,
	  // C(cones(system))
      {

	const auto& J = system.constraint.bilateral.matrix;
	const auto& c = system.constraint.bilateral.values;
      
	const auto& N = system.constraint.unilateral.matrix;
	const auto& n = system.constraint.unilateral.values;
      
	const auto& T = system.constraint.friction.matrix;
	const auto& b = system.constraint.friction.bounds;
      

	mosek->add( dynamics );

	if( bilateral.dim() ) { 
	  // core::log()("got bilateral lol");
	  mosek->add(lambda);
	  AT<constraint::bilateral>(*mosek, J, dynamics, lambda, num.dofs, -dt);
	}

	if( unilateral.dim() ) { 
	  mosek->add(mu);
	  mosek->bounds(mu, math::vec::Zero( mu.dim() ), math::mosek::bound::lower );
	  AT<constraint::unilateral>(*mosek, N, dynamics, mu, num.dofs, -dt);
	}

	// if( num.friction.dim() ) { 
	//   mosek->add(nu);
	  
	//   core::log()("got friction lol");
	  
	//   mosek->bounds(nu, math::vec::Zero( nu.dim() ), math::mosek::bound::lower );
	//   AT<constraint::friction>(*mosek, T, dynamics, nu, num.dofs, -dt);
	// }

	A(*mosek, system.mass, dynamics, v);
	
      }


      dof::velocity precise::operator()(const dof::momentum& f) const {
	using sparse::linear::operator-;
	c(*mosek,  -f, v, num.dofs);

	mosek->b( dynamics, num.dofs.dense(f), math::mosek::bound::fixed );
	return direct::operator()(f);
	
      }



    }
  }
}
