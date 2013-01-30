#include "balance.h"

#include <phys/skel/momentum.h>
#include <phys/skel/com.h>
#include <phys/skel/markers.h>

#include <core/tree/algo.h>

#include <math/func/jacobian.h>
#include <math/func/index.h>

#include <math/func/coord.h>
#include <math/func/tuple.h>
#include <math/func/euclid.h>

namespace control {
  namespace obj {

    using namespace math;

    balance::balance(const phys::skeleton& skeleton)
      : constraint( 3 ),
	skeleton(skeleton), 
	height(16),
	com(vec3::Zero()),
	dir(vec3::Zero()),
	com_body(vec3::Zero()),
	lf(0),
	rf(0),
	weight(1)
    {  
      pd.p(1).d(0);
    }
    
    
    phys::constraint::bilateral::matrix balance::matrix(const phys::skel::pose& at) const {
      assert(lf && rf );

      phys::constraint::bilateral::matrix res;
      
      mat L = // phys::skel::momentum::linear(skeleton, at) / skeleton.total_mass;
	func::J( phys::skel::com( skeleton ), math::lie::of(at), math::Lie< math::vec3>() )( at );
      
      mat Lv = L.row(1);
      mat Lh; Lh.resize(2, L.cols());
      Lh << 
	L.row(0), 
	L.row(2);

      
      auto markers = phys::skel::markers(skeleton);
     
      markers.indices.insert(lf);
      markers.indices.insert(rf);

      // feet jacobian
      const mat Feet = func::J(markers, markers.dmn(), markers.rng())(at);

      // midfeet jacobian
      const mat Mid = 0.5 * (Feet.topRows(3) + Feet.bottomRows(3));
      
      // horizontal projection
      mat Proj; Proj.resize(2, Mid.cols());
      Proj << 
	Mid.row(0),
	Mid.row(2);
      
      // full constraint matrix: 1st: vertical com, 2nd: horizontal midfeet - com
      math::mat full; full.resize(constraint.dim, Feet.cols());
      full << 
	weight * Lv,
	weight * (Proj - Lh);
      
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
    
    phys::constraint::bilateral::values balance::values(const math::T<phys::skel::pose>& dpose,
							math::real t,
							math::real dt) const {
      phys::constraint::bilateral::values res;
      

      auto markers = phys::skel::markers(skeleton);
     
      markers.indices.insert(lf);
      markers.indices.insert(rf);
      
      using namespace func;
      using func::operator+;
      
      const Lie< phys::skel::markers::range > mrk(markers.indices.size());
      
      auto dmid = (0.5 * (coord(0, mrk) + coord(1, mrk) ) << markers) ( dpose ); // midfoot 
      
      auto dcom = phys::skel::com( skeleton )( dpose );
      com = dcom.at();
      com_body = dcom.body();

      vec3 at, body;
      at.x() = dcom.at().y();
      body.x() = dcom.body().y();

      at.y() = dmid.at().x() - dcom.at().x();
      body.y() = dmid.body().x() - dcom.body().x();
      
      at.z() = dmid.at().z() - dcom.at().z();
      body.z() = dmid.body().z() - dcom.body().z();

      target = vec3( dmid.at().x(), height, dmid.at().z() );
      

      dir = pd(math::body(at, body), vec3(height, 0, 0));
      dir(0) = 0;

      // std::cout << "balance error: " << (weight*(at - vec3(height, 0, 0))).squaredNorm() << std::endl;


      using namespace math;
      res[&constraint] = weight * dir;
      
      return res;
    }

  }
}

