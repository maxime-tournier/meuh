#ifndef GEO_PLANE_H
#define GEO_PLANE_H

#include <math/vec.h>

namespace geo {

  struct plane {
    
    math::vec4 equation;

    plane();

    void set(const math::vec3& origin,
	     const math::vec3& normal);

    bool collide(const math::vec3& ) const;

    // signed euclidean distance, negative if inside
    math::real dist(const math::vec3& ) const;

    // outwards normal
    math::vec3 normal() const;
    
    // some point on the plane
    math::vec3 origin() const;
    
    math::vec3 proj(const math::vec3& v) const;

  };



}




#endif
