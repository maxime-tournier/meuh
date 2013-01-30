#include "direct.h"

#include <phys/system.h>

#include <sparse/zero.h>
#include <sparse/zip.h>
#include <sparse/linear.h>

#include <phys/error.h>

#include <core/log.h>

#include <phys/solver/qp.h>

#include <sparse/linear/transpose.h>

namespace phys {
  namespace solver {

    namespace qp { 


      void direct::correct(bool value) {
	const auto& c = value ? system.constraint.bilateral.corrections : system.constraint.bilateral.values;
	const auto& n = system.constraint.unilateral.values;
	
	if( bilateral.dim() ) {
	  mosek->b(bilateral, num.bilateral.dense(c), math::mosek::bound::fixed );
	}

	if( unilateral.dim() ) { 
	  mosek->b(unilateral, num.unilateral.dense(n), math::mosek::bound::lower );
	}

      }


    
      direct::direct(const phys::system& system, math::real dt) 
	: system(system),
	  num(system),
	
	  v(0, num.dofs.dim() ),
	  
	  bilateral(0, num.bilateral.dim()),
	  unilateral(bilateral.after(num.unilateral.dim())),
	  // friction(unilateral.after(num.friction.dim())),
	
	  mosek( core::plugin::load<plugin::mosek>() )
	  
      {
	const auto& J = system.constraint.bilateral.matrix;
	const auto& N = system.constraint.unilateral.matrix;

	const auto& T = system.constraint.friction.matrix;
	const auto& b = system.constraint.friction.bounds;
      
	// const auto& C = cones(system);
      
	mosek->init();
      
	// mosek->debug( true ); 	

	mosek->add(v);
	mosek->add(bilateral).add(unilateral)// .add(friction)
	  ;
      
	using namespace math;
      
	if( bilateral.dim() ) {
	  A<constraint::bilateral>(*mosek, J, bilateral, v, num.dofs);
	}

	if( unilateral.dim() ) { 
	  A<constraint::unilateral>(*mosek, N, unilateral, v, num.dofs);
	}

	correct( false );
	
	Q(*mosek, system.mass, v, num.dofs);

	// viscous friction
	if( false && num.friction.dim() ) { 
	  core::log()("got friction lol");
	  // A<constraint::friction>(*mosek, C, friction, v, num.dofs);
	  // mosek->b(friction, vec::Zero(friction.dim()), mosek::bound::lower );

	  using namespace sparse::linear;

	  auto Tt = transp(T).full();
	  auto TtT = Tt * transp(Tt);
	  
	  real alpha = 1e5;
	  
	  add_Q(*mosek, TtT, v, num.dofs, dt * alpha);
	}
	
	
      }
	
    

      dof::velocity direct::operator()(const dof::momentum& f) const {
 	using sparse::linear::operator-;
	c(*mosek, -f, v, num.dofs);
      
	return num.dofs.sparse( mosek->solve(v) );
      }

    }


  }
}
