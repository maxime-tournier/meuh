#include "inertia.h"


#include <math/vec.h>
#include <math/matrix.h>


namespace phys {
  namespace rigid {
    namespace inertia {
	
      using namespace math;

      vec3 ellipsoid(const vec3& dimensions){
	assert( dimensions.norm() > 0 );

	const vec3 radius = dimensions / 2;

	const real x_2 = radius.x() * radius.x();
	const real y_2 = radius.y() * radius.y();
	const real z_2 = radius.z() * radius.z();
	  
	return 1.0 / 5.0 * vec3( y_2 + z_2, 
				 x_2 + z_2, 
				 x_2 + y_2 );
      }
     
     
      vec3 cylinder(const vec3& dimensions) {
	assert( dimensions.norm() > 0 );
  
	const vec3 radius = dimensions / 2;

	const real x_2 = radius.x() * radius.x();
	const real y_2 = radius.y() * radius.y();
	const real z_2 = radius.z() * radius.z();

	return 1.0 / 12.0 * vec3( y_2 + z_2, x_2 + z_2, x_2 + y_2 );
	  
      }
    }

  }
}
