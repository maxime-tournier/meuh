#include "head.h"

#include <core/tree/algo.h>
#include <math/func/jacobian.h>

#include <math/func/coord.h>
#include <math/func/tuple.h>
#include <math/func/euclid.h>

#include <phys/skel/markers.h>

namespace control {
  namespace obj {
    using namespace math;

    head::head(const phys::skeleton& skeleton) 
      : constraint( 3 ),
	skeleton(skeleton), 
	height(20),
	weight(1),
	index(0),
	lf(0),
	rf(0) {
      pd.p(1).d(0);
    }
    
    phys::constraint::bilateral::matrix head::matrix(const phys::skel::pose& at) const {
      assert(index);
      
      phys::constraint::bilateral::matrix res;


      // head
      auto head = phys::skel::markers(skeleton);
      head.indices.insert(index);

      const mat Head = func::J(head, head.dmn(), head.rng())( at );

      const mat Hv = Head.row(1);
      mat Hh; Hh.resize(2, Head.cols());
      Hh << Head.row(0), Head.row(2);

      // feet
      auto feet = phys::skel::markers(skeleton);
     
      feet.indices.insert(lf);
      feet.indices.insert(rf);

      // feet jacobian
      const mat Feet = func::J(feet, feet.dmn(), feet.rng())(at);

      // midfeet jacobian
      const mat Mid = 0.5 * (Feet.topRows(3) + Feet.bottomRows(3));
      
      // horizontal projection
      mat Proj; Proj.resize(2, Mid.cols());
      Proj << 
	Mid.row(0),
	Mid.row(2);
      
      // full constraint matrix: 1st: vertical head, 2nd: horizontal midfeet - head
      math::mat full; full.resize(constraint.dim, Feet.cols());
      full << 
	weight * Hv,
	weight * (Proj - Hh);
      
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


     phys::constraint::bilateral::values head::values(const math::T<phys::skel::pose>& dpose,
							math::real t,
							math::real dt) const {
       assert(index);
       
       phys::constraint::bilateral::values res;
       
       // head
       auto head = phys::skel::markers(skeleton);
       head.indices.insert(index);
       
       const Lie< phys::skel::markers::range> hd(head.indices.size());
       
       auto dhead = (func::coord(0, hd) << head)(dpose);
       

       // feet
       auto feet = phys::skel::markers(skeleton);
     
       feet.indices.insert(lf);
       feet.indices.insert(rf);
       
       const Lie< phys::skel::markers::range> ft(feet.indices.size());
      

       using namespace func;
       using func::operator+;
       auto dmid = (0.5 * (coord(0, ft) + coord(1, ft) ) << feet) ( dpose ); // midfoot 
       
       
       vec3 at, body;
       at.x() = dhead.at().y();
       body.x() = dhead.body().y();
       
       at.y() = dmid.at().x() - dhead.at().x();
       body.y() = dmid.body().x() - dhead.body().x();
       
       at.z() = dmid.at().z() - dhead.at().z();
       body.z() = dmid.body().z() - dhead.body().z();
       
       const vec3 dir = pd(math::body(at, body), vec3(height, 0, 0));
       
       res[&constraint] = weight * dir;
       return res;

     }

    
  }
}
