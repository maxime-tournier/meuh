#include "feet_distance.h"

#include <phys/skel/momentum.h>
#include <phys/skel/com.h>
#include <phys/skel/markers.h>

#include <core/tree/algo.h>

#include <math/func/jacobian.h>
#include <math/func/index.h>


namespace control {
  namespace obj {

    using namespace math;

    feet_distance::feet_distance(const phys::skeleton& skeleton)
      : constraint( 1 ),
	skeleton(skeleton), 
	target( 6 ),
	lf(0),
	rf(0),
	lt(0),
	rt(0),
	weight(1)
    { 
      pd.p(1).d(0);
    }
    
    
    phys::constraint::bilateral::matrix feet_distance::matrix(const phys::skel::pose& at) const {
      phys::constraint::bilateral::matrix res;
      
      assert(lf && rf && rt && lt);

      auto feet = phys::skel::markers(skeleton);
      feet.indices.insert(lf);
      feet.indices.insert(rf);

      auto toes = phys::skel::markers(skeleton);
      toes.indices.insert(lt);
      toes.indices.insert(rt);
    
      auto feet_pos = feet(at);
      auto feet_diff = feet_pos(0) - feet_pos(1);
      
      auto toes_pos = toes(at);
      auto toes_diff = toes_pos(0) - toes_pos(1);
      
      // if(feet_diff.norm() > target) return res;
      using namespace math;

      const mat Feet = func::J(feet, feet.dmn(), feet.rng())(at);
      const mat Toes = func::J(toes, toes.dmn(), toes.rng())(at);
      
      const mat Dfeet = Feet.topRows(3) - Feet.bottomRows(3);
      const mat Dtoes = Toes.topRows(3) - Toes.bottomRows(3);
      
      mat full; full.resize(constraint.dim, Dfeet.cols());
      full << 
	weight * 2 * feet_diff.transpose() * Dfeet;
	// weight * 2 * toes_diff.transpose() * Dtoes;
	
      skeleton.topology->each([&](phys::skel::topology* j) {
	  // const math::mat33 R = at(j->get().id).rotation().inverse().matrix();
	  
	  // math::mat66 to_body;
	  // to_body.block<3, 3>(0, 3) = R;
	  // to_body.block<3, 3>(3, 0) = R;
	  // to_body.block<3, 3>(0, 0).setZero();
	  // to_body.block<3, 3>(3, 3).setZero();
	  
	  res[&constraint][ j->get().dof ] = full.block(0, 6*j->get().id,
							constraint.dim, 6) // * to_body
	    ;
	  
	});
      
      return res;
    }
    
    phys::constraint::bilateral::values feet_distance::values(const math::T<phys::skel::pose>& dpose,
							math::real t,
							math::real dt) const {
      phys::constraint::bilateral::values res;
      
      using namespace math;

      auto feet = phys::skel::markers(skeleton);
      feet.indices.insert(lf);
      feet.indices.insert(rf);

      auto dfeet = feet(dpose);
      auto dfeet_diff = math::body<math::vec3>( dfeet.at()(0) - dfeet.at()(1),
						dfeet.body()(0) - dfeet.body()(1) );
      // if( dfeet_diff.at().norm() > target ) return res;

      auto dfeet_norm = math::body(dfeet_diff.at().squaredNorm(), 
				   2 * dfeet_diff.at().dot(dfeet_diff.body()));
      
      auto toes = phys::skel::markers(skeleton);
      toes.indices.insert(lt);
      toes.indices.insert(rt);

      auto dtoes = toes(dpose);
      auto dtoes_diff = math::body<math::vec3>( dtoes.at()(0) - dtoes.at()(1),
      						dtoes.body()(0) - dtoes.body()(1) );
      
      auto dtoes_norm = math::body(dtoes_diff.at().squaredNorm(), 
				   2 * dtoes_diff.at().dot(dtoes_diff.body()));


      math::vec1 dir;
      dir(0) = pd(dfeet_norm, target * target);
      // dir(0) = pd(dtoes_norm, target * target);
      
      using namespace math;
      res[&constraint] = weight * dir;
      
      return res;
    }

  }
}

