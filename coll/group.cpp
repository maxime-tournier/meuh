#include "group.h"

#include <core/stl.h>
#include <coll/primitive.h>


namespace coll {

  hit::list group::collide(const group& other) const {
    hit::list res;

    core::each(primitives, [&](const primitive* mine) {
	core::each(other.primitives, [&](const primitive* theirs) {
	    const auto hits = mine->collide(*theirs);
	    res.insert(res.end(), hits.begin(), hits.end());
	  });
      });
      
    return res;
  }
    

}
