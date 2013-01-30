#ifndef PHYS_OBJECT_H
#define PHYS_OBJECT_H

#include <phys/dof.h>

#include <math/lie.h>
#include <math/eigen.h>

#include <math/func/pull.h>

#include <phys/error.h>
#include <core/macro/derp.h>

// default container for what's needed *for integration*
namespace phys {
  
  struct approx {
    
    enum { ugly = 0, bad = 1, good = 2 };
    static math::natural level;
  };
  
  
  template<class G>
  struct object : dof {
    
    // TODO is this even needed ?
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    // configuration space 
    typedef math::lie::group<G> lie_type;
    const lie_type lie;
    
    typedef G config_type;
    typedef typename lie_type::algebra velocity_type;
    typedef typename lie_type::coalgebra momentum_type;

    // TODO rename state ?
    typedef std::tuple<config_type, 
		       velocity_type,
		       momentum_type> bundle_type;
    bundle_type bundle;

    // body-fixed velocity/momentum
    config_type& g() { return std::get<0>(bundle); }
    velocity_type& v() { return std::get<1>(bundle); }
    momentum_type& p() { return std::get<2>(bundle); }

    const config_type& g() const { return std::get<0>(bundle); }
    const velocity_type& v() const { return std::get<1>(bundle); }
    const momentum_type& p() const { return std::get<2>(bundle); }
    
    object(const object& ) = default; 
    object(object&& ) = default;
        
    void reset() { 
      
      g() = lie.id(); 
      v() = lie.alg().zero();
      p() = lie.coalg().zero();
      
      delta_conf = lie.id();
      previous_velocity = lie.alg().zero();
    }
    
    object(const math::lie::group<G>& lie = math::lie::group<G>()) 
      : dof( lie.alg().dim() ),
	lie( lie )
    {
      reset();
    }

    // returns net momentum for system solving (momentum + external impulse)
    momentum_type net(const momentum_type& f, math::real dt) const {
      
      // external impulse
      const momentum_type ext = lie.coalg().scal(dt, f);

      // net external impulse
      using namespace math::func;
      
      const momentum_type rhs = lie.coalg().sum( dT(lie.log())(delta_conf)( p() ), ext );
      
      switch(approx::level) {
	
      case approx::ugly: return lie.coalg().sum(p(), ext );
      case approx::bad:  return rhs;
      case approx::good: {
	const velocity_type pred_delta_velocity = lie.alg().scal(dt, prediction());
	const config_type   pred_delta_conf = lie.exp()( pred_delta_velocity );
	return dT(lie.exp())(pred_delta_velocity) ( lie.adT( pred_delta_conf )( rhs ) ) ;
      }
      };
      
      throw phys::error( DERP );
    }

    void correct(const velocity_type& delta) {
      
      config_type d = lie.exp()( delta );
      
      g() = lie.prod(g(), d);
      delta_conf = lie.prod(delta_conf, d);
      
    }


    // integrates conf and momentum given velocity and momentum
    void integrate(const velocity_type& vel, 
		   const momentum_type& mu,
		   math::real dt) {
      previous_velocity = v();
      v() = vel;
      p() = mu;
      
      const velocity_type delta_velocity = lie.alg().scal(dt, vel);
      delta_conf = lie.exp()( delta_velocity );
      
      g() = lie.prod(g(), delta_conf);
    }
    
    // velocity prediction
    velocity_type prediction() const {
      switch(approx::level) {
	// linear prediction
      case approx::good: {
	const auto alg = lie.alg();
	return alg.sum( alg.scal(2, v()), alg.scal( -1, previous_velocity) );
      }
      default: 
	// constant velocity
	return v();
      }
    }
    

    typedef std::function< momentum_type (const velocity_type& ) > pair_type;
    pair_type pair;
    
  private:
    config_type delta_conf;
    velocity_type previous_velocity;
  };


}



#endif
