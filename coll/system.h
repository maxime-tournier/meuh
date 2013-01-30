#ifndef COLL_SYSTEM_H
#define COLL_SYSTEM_H

#include <map>
#include <functional>
#include <math/types.h>

#include <coll/group.h>


namespace coll {

  struct primitive;
  struct hit;
    
  struct system {

    typedef std::function<void(math::natural, const hit& )> callback;

    typedef std::map< const primitive*, callback > primitives_type;
    primitives_type primitives;
      
    math::natural detect() const;

    // TODO !
    std::list< group > groups;
  };

}




#endif
