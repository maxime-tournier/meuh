#include "si.h"
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

    si::si(const solver::compliance& K,
	   const math::algo::stop& stop)
      : K(K),
	stop(stop),
	mu(make_mu(K)),
	normal_map(make_normal_map(K))
    {
      
    }
    
    static const core::log log;
    
    dof::velocity si::operator()(const dof::momentum& f, math::vec lambda) const {
      
      const dof::velocity resp = K.response(f);

      if(!K.dim()) return resp;

      using namespace math;
      const vec rhs = K.rhs( resp );
      
      if( lambda.empty() ) lambda.setZero( K.dim() );
      assert( lambda.rows() == K.dim() );

      const real epsilon = 1;

      assert( (K.r % 2) == 0 );
      algo::iter_eps(stop, [&](real& eps) { 
	  real error = 0;
	  rhs.each([&](natural i ) {
	      const real prev = lambda(i);

	      lambda(i) += ( rhs(i) - K.row_dot(i, lambda) ) / 
		( K.diagonal(i) //  + epsilon // + K.damping(i)
		  );
	      
	      if( (i >= K.p) && (i < (K.p + K.q)) ) {
		lambda(i) = std::max(0.0, lambda(i));
	      }
	      
	      const real diff = (lambda(i) - prev);
	      error += diff * diff;

	      // TODO project friction
	    });

	  eps = std::sqrt(error);
	
	});
      
      assert(!nan(lambda));
      // log("si lambda: ", lambda.transpose() );


      using sparse::linear::operator+;
      return resp + K.correction(lambda);
    }

    
  }
}


 // if( friction  && i >= (K.p + K.q) ) {
	      // 	const natural r = i - K.p - K.q;
	      // 	const natural n = normal_map(r);
		
	      // 	// FIXME
	      // 	const natural other = r % 2 ? i - 1 : i + 1;
	      // 	const real lambda_other = lambda(other);
	      // 	const real lambda_n = lambda(K.p + n);
		
	      // 	if(  lambda_other * lambda_other + lambda(i) * lambda(i) >= mu(n) * lambda_n * lambda_n ) {
	      // 	  // clamp to cone
	      // 	  // assert( lambda_other * lambda_other <= mu(n) * lambda_n * lambda_n);
	      // 	  vec2 tangent; tangent << lambda(i), lambda(other);
		  
	      // 	  tangent = mu(n) * tangent.normalized();
		  
	      // 	  lambda(i) = tangent(0);
	      // 	}
	      // }
