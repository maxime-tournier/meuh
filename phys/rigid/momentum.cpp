#include "momentum.h"

#include <phys/rigid/com.h>

namespace phys {				
  namespace rigid {

    namespace momentum {
      using namespace math;

      matrix<math::real, 3>  angular(const rigid::skeleton& skel, const config& at) {
	const natural n = skel.size();
	assert( at.rows() == n );
      
	mat res = mat::Zero(3, 6*n);
	
	const vec3 c = com(skel) ( at );
	
	skel.each( [&](math::natural i) {
	    res.block<3, 3>(0, 6*i) = at(i).rotation().matrix() * skel.topology[i].inertia.asDiagonal();
	  
	    const vec3 ri = at(i).translation()  - c;
	  
	    res.block<3, 3>(0, 6*i + 3) = skel.topology[i].mass * math::hat( ri )
	      * at(i).rotation().matrix();
	  });
      
      
	return std::move(res);

      }

      matrix<math::real, 3>  linear(const rigid::skeleton& skel, const config& at) {
	const natural n = skel.size();
	assert( at.rows() == n );
      
	mat res = mat::Zero(3, 6*n);
      
	skel.each( [&](math::natural i) {
	    res.block<3, 3>(0, 6*i + 3) = skel.topology[i].mass * at(i).rotation().matrix();
	  });

	return std::move(res);
       
      }
     

    }
  }
}
