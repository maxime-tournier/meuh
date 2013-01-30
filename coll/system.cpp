#include "system.h"

#include <coll/primitive.h>
#include <core/stl.h>


namespace coll {

  math::natural system::detect() const {

    math::natural count = 0;
      
    // good ol'quadratic test
    for(auto first = primitives.begin(), end = primitives.end(); first != end; ++first){
      for(auto tmp = first, second = ++tmp; second != end; ++second) {
	  
	// collide primitives
	const auto hits = first->first->collide(*second->first);
	  
	// process hits by both sides
	core::each(hits, [&](const hit& h) {
	    first->second(count, h);
	    second->second(count, h);
	    ++count;
	  });
      }
    }
      
    return count;
  }


}
