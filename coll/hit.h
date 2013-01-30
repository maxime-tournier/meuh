#ifndef COLL_HIT_H
#define COLL_HIT_H

#include <math/vec.h>
#include <list>

namespace coll {
    
  struct primitive;

  struct hit {
    typedef std::list<hit> list;

    const primitive* first;
    const primitive* second;
      
    const math::vec3 pos;
    const math::vec3 normal; 	// first -> second

    const math::real error;

    // direction to maintain non-penetration
    math::real sign(const primitive* p) const;

    // convenience: sign(p) * normal
    math::vec3 dir(const primitive* p) const; 
  };

}


#endif
