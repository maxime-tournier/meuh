#ifndef GEO_AABB_H
#define GEO_AABB_H

#include <math/vec.h>

namespace geo {

  struct aabb {
    math::vec3 lower, upper;
    
   

    bool empty() const;

    math::vec3 center() const;
    math::vec3 dim() const;
    math::vec3 radius() const;
    
    aabb();


    // union
    aabb& operator+=(const math::vec3& );
    aabb& operator+=(const aabb& );

    void clear();

    template<class F>
    void each(const F& f) const {
      auto c = center();
      auto r = radius();

      for(int i = -1; i <= 1; i += 2)
	for(int j = -1; j <= 1; j += 2)
	  for(int k = -1; k <= 1; k += 2)
	    f( math::vec3(c.x() + i * r.x(),
			  c.y() + j * r.y(),
			  c.z() + k * r.z()) );
    };
    
  };

}


#endif
