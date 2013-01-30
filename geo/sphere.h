#ifndef GEO_SPHERE_H
#define GEO_SPHERE_H

#include <math/vec.h>

namespace geo {

  struct sphere {
    math::vec3 center;
    math::real radius;
  
    bool contains(const math::vec3& ) const;
    math::vec3 proj(const math::vec3& ) const;
    math::real dist(const math::vec3& ) const;

    math::vec3 normal(const math::vec3& ) const;
    
  };


}



#endif
