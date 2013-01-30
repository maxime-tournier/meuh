#include "offset.h"

#include <math/real.h>
#include <math/vec.h>

namespace mocap {

  // in the case of a joint with multiple children, returns the one
  // with non-zero offset (corresponds to an actual rigid body)
  static mocap::skeleton* bone_end(mocap::skeleton* j) {
    assert( j );
    
    mocap::skeleton* res = 0;
    j->children( [&](mocap::skeleton* c) {
	  
	if( c->get().offset.norm() > math::epsilon ) {
	  assert( !res && "several children with non zero offset");
	  res = c;
	}
      });
      
    return res;
  }
    

  math::vec3 offset(mocap::skeleton* j) { 
    mocap::skeleton* c = bone_end(j);

    if(!c) return math::vec3::Zero();
    
    return c->get().offset; 
  }
  
  math::real length(mocap::skeleton* j) { return offset(j).norm(); }
  
}
