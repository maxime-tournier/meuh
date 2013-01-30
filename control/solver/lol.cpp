#include "lol.h"

#include <math/algo/iter.h>
#include <sparse/linear.h>

#include <core/log.h>
#include <phys/solver/si.h>
#include <phys/solver/vsi.h>

#include <math/mosek.h>

namespace control {
  namespace solver {

    
    lol::lol(const phys::system& system, math::real dt, math::real epsilon)
      : cache(system, dt),
	epsilon(epsilon)
    {
      
    }
    


    

    phys::dof::velocity lol::operator()(const phys::dof::momentum& f,
					  control::actuator::vector& act,
					   const objective& obj) const {
      using namespace math;

      math::mosek mosek;

      mosek.init(cache.n + cache.p + cache.q + cache.r + 2 * cache.q,
		 cache.n + cache.p + cache.q + cache.r + cache.q);

      const math::mosek::variable v(0, cache.n);
      const math::mosek::variable lambda(v.end, v.end + cache.p);
      const math::mosek::variable mu(lambda.end, lambda.end + cache.q);
      const math::mosek::variable nu(mu.end, mu.end + cache.r);
      
      const math::mosek::variable z(nu.end, nu.end + cache.q );
      const math::mosek::variable mum(z.end, z.end + cache.q );

      const math::mosek::constraint dynamics(0, cache.n);
      const math::mosek::constraint bilateral(dynamics.end, dynamics.end + cache.p);
      const math::mosek::constraint unilateral(bilateral.end, bilateral.end + cache.q);
      const math::mosek::constraint friction(unilateral.end, unilateral.end + cache.r);
      
      const math::mosek::constraint slack(friction.end, friction.end + cache.q );
      
      // FIXME
      const mat& M = cache.mass.begin()->second;

      mosek.A(dynamics, v, M);
      if( cache.p ) mosek.A(dynamics, lambda, -cache.J.dense.transpose());
      if( cache.q ) mosek.A(dynamics, mu, -cache.N.dense.transpose());
      // if( cache.r ) mosek.A(dynamics, nu, -cache.T.dense.transpose());
      mosek.b(dynamics, cache.dofs.dense(f), math::mosek::bound::fixed );

      if( cache.p ) {
	mosek.A(bilateral, v, cache.J.dense);
	mosek.b(bilateral, cache.J.keys.dense(cache.system.constraint.bilateral.values), 
		math::mosek::bound::fixed );
      }

      if( cache.q ) {
	mosek.A(unilateral, v, cache.N.dense);
	mosek.A(unilateral, mum, -mat::Identity(mum.dim(), mum.dim()));
	
	mosek.b(unilateral, cache.N.keys.dense(cache.system.constraint.unilateral.values),
		math::mosek::bound::fixed );
	
	mosek.bounds(mu, vec::Zero(mu.dim()), math::mosek::bound::lower );
	mosek.bounds(mum, vec::Zero(mum.dim()), math::mosek::bound::lower );

	mosek.A(slack, z, mat::Identity(slack.dim(), z.dim()));
	mosek.A(slack, mu, -mat::Identity(slack.dim(), mu.dim()));
	mosek.A(slack, mum, mat::Identity(slack.dim(), mum.dim()));
	
	mosek.b(slack, vec::Zero(slack.dim()), math::mosek::bound::fixed);
      }
      
      // if( cache.r ) {
      // 	mosek.A(friction, v, cache.T.dense);
      // 	mosek.b(friction, vec::Zero(nu.dim()),
      // 		math::mosek::bound::fixed);
      // }
      
      if( cache.q ) {
	const real weight = 1.0;
	mosek.Q(mu, weight * mat::Identity(mu.dim(), mu.dim()));
	mosek.Q(mum, weight * mat::Identity(mum.dim(), mum.dim()));
      }

      // mosek.Q(v, M);
      // mosek.c(v, -cache.dofs.dense(f));
      try {
	return cache.dofs.sparse( mosek.solve(v) );
      } catch( const math::mosek::unknown_status& e) {
	return cache.dofs.sparse(e.result);
      }

    }



  }
}
