#ifndef COLL_GROUP_H
#define COLL_GROUP_H

#include <list>
#include <coll/hit.h>

namespace coll {

  struct primitive;

  // groups hold primitives that wont be collided between each
  // other.
  struct group {
      
    std::list< const primitive* > primitives;

    // good old quadratic collision check
    hit::list collide(const group& ) const;
      
  };

}

#endif
