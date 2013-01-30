#include <sparse/vector.h>
#include <sparse/matrix.h>

#include "system.h"

#include <math/vec.h>
#include <math/matrix.h>

#include <math/pi.h>
#include <phys/error.h>
#include <core/unique.h>

#include <core/log.h>
#include <sparse/zero.h>

namespace phys {
  


  constraint::bilateral::pack system::constraint_type::bilateral_type::pack() const {
    return { matrix, values };
  }


  constraint::unilateral::pack system::constraint_type::unilateral_type::pack() const {
    return { matrix, values };
  }



  constraint::friction::pack system::constraint_type::friction_type::pack() const {
    return { matrix, bounds };
  }
  
  void system::clear() {
    *this = system();
  }

  
  struct discretize_impl {

    const math::real mu;	// coulomb coeff
    const math::natural n;
  
    math::mat operator()(const math::mat& normal, const math::mat& tangent) const {
      assert(normal.rows() == 1);
      assert(tangent.rows() == 2);
      assert( normal.cols() == tangent.cols() );

      using namespace math;

      mat res = mat::Zero(n, normal.cols() );
      
      res.each_row([&](natural i){

	  real alpha = (2.0 * pi * real(i) ) / real(n);
	  res.row(i) = std::cos(alpha) * tangent.row(0) + std::sin(alpha) * tangent.row(1)
	    + normal / mu;
	  
	});
      
      return res;
    }

    math::mat operator()(const math::mat& , sparse::zero ) const { 
      throw phys::error("normal/tangent dof mismatch");
    }

    math::mat operator()(sparse::zero, const math::mat& ) const { 
      throw phys::error("normal/tangent dof mismatch");
    }

  };

  // TODO broken by sparse
  // void system::constraint_type::discretize(math::natural n) {
  //   assert( discrete.empty() );
    
  //   std::vector< phys::constraint::unilateral::key > to_erase;
  //   to_erase.reserve(cones.size());
    
  //   core::each(cones, [&](phys::constraint::cone::key c) {
	
  // 	auto k = new const phys::constraint::unilateral( n );
  // 	unilateral.storage.push_back( core::unique(k) );
	
  // 	sparse::zip( unilateral.matrix[ k ],
  // 		     unilateral.matrix.find(c->normal)->second,
  // 		     friction.matrix.find(c->tangent)->second,
  // 		     discretize_impl{c->mu, n} );
	
  // 	discrete[ c ] = k;
    
  // 	to_erase.push_back( c->normal );
  //     });

  //   // erase normal
  //   core::each( to_erase, [&]( phys::constraint::unilateral::key n) {
  // 	unilateral.matrix.erase( n );
  // 	unilateral.values.erase( n );
  //     });
  // }



  // TODO broken by sparse stuff
  // phys::constraint::unilateral::vector system::constraint_type::rhs(const phys::dof::velocity& v) const {
    
  //   phys::constraint::unilateral::vector res;
    
  //   using sparse::linear::operator*;

  //   // tangent velocity
  //   const phys::constraint::friction::vector t = friction.matrix * v;

  //   core::each(discrete, [&](phys::constraint::cone::key c,
  // 			     phys::constraint::unilateral::key k) {
  // 		 auto it = t.find( c->tangent ); assert( it != t.end() );
  // 		 const math::real s = it->second.norm();
		 
  // 		 res[ k ].setConstant( k->dim,  s );
  // 		 // res[ c->normal ].setConstant(1, c->mu * s);

  // 	       });

  //   return res;
  // }

} 
