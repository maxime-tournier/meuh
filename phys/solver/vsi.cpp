#include "vsi.h"
#include <math/algo/iter.h>
#include <sparse/linear.h>

#include <core/log.h>

namespace phys {
  namespace solver {


    static math::vec make_mu(const solver::compliance& K) {
      math::vec res; 
      if( K.system.constraint.cones.empty() ) return res;
      
      res.resize(K.system.constraint.cones.size());
      phys::constraint::unilateral::vector mu;
      
      core::each(K.system.constraint.cones,
		 [&](const phys::constraint::cone* c) {
		   mu[c->normal].setConstant(1, c->mu);
		 });
      
      return K.N.keys.dense( mu );
    }

    // maps tangent indices to normal indices
    static math::vector<math::natural> make_normal_map(const solver::compliance& K) {
      math::vector<math::natural> res; res.resize(K.T.keys.dim());
      
      core::each(K.system.constraint.cones, [&](const phys::constraint::cone* c) {
	  res.segment(K.T.keys.off(c->tangent), c->tangent->dim).setConstant( K.N.keys.off(c->normal));
	});
      
      return res;
    }

    vsi::vsi(const solver::compliance& K,
	     const math::algo::stop& stop)
      : K(K),
	stop(stop),
	mu(make_mu(K)),
	normal_map(make_normal_map(K))
    {
      
    }
    

   

    // fishy: we dont have access to total lambdas
    math::real vsi::project(const math::vec& x, math::natural i) const {
      using namespace math;
      // bilateral
      if( i < K.p ) return x(i);
	
      // unilateral 
      if( i >= K.p && i < K.p + K.q ) {
	return std::max(0.0, x(i) );
      }

      // friction
      assert(i < K.p + K.q + K.r );
	
      const natural r = i - K.p - K.q;
      const natural n = normal_map(r);
      
      // FIXME (contiguous, binary friction constraints)
      const natural other = r % 2 ? i - 1 : i + 1;
      const real x_other = x(other);
      const real x_n = x(K.p + n);
	
      if(  x_other * x_other + x(i) * x(i) >= mu(n) * x_n * x_n ) {
	// clamp to cone
	vec2 tangent; tangent << x(i), x(other);
	
	tangent = mu(n) * tangent.normalized();
	  
	return tangent(0);
      }
	
      return x(i);
    }


    dof::velocity vsi::operator()(const dof::momentum& f) const {
      
      const dof::velocity resp = K.response(f);
      
      using namespace math;
      const vec rhs = K.rhs( dof::velocity() );

      vec v = K.dofs.dense(resp);
      vec vc = vec::Zero(v.rows());
      vec delta;
      
      const real epsilon = 1e-14;
      core::log log;

      // FIXME
      const mat& M = K.system.masses.begin()->second;

      real error = 0;

      math::natural it = algo::iter_eps(stop, [&](real& eps) { 
	  error = 0;
	  
	  rhs.each([&](natural i) {
	      
	      real lambda = ( rhs(i) - K.dense(i).dot(v) ) / 
	      	(K.diagonal(i) + K.damping(i) + epsilon );
	       
	      // unilateral
	      if( i >= K.p && i < K.p + K.q ) {
	      	// // current lambda (fail)
		// const real total = vc.dot(K.linear(i));
		// lambda = std::max(-total, lambda);
		lambda = std::max(0.0, lambda);
	      }
	      
	      if( lambda != 0 ) {

		// TODO friction
		delta = lambda * K.linear(i); 
		assert(!nan(delta));
	      
		v += delta;
		vc += delta;
	      
		error += lambda * lambda;
	      }
	      
	    });
	  log("error: ", error);
	  
	  eps = error;
	}).iterations;
      std::cout << std::endl;
      log("vsi iterations: ", it, " energy: ", 0.5 * vc.dot(M*vc));
      
      return K.dofs.sparse(v);
    }

    
  }
}
