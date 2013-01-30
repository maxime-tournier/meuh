#include "sphere.h"

namespace geo {

  bool sphere::contains(const math::vec3& v) const {
    return (v - center).squaredNorm() <= (radius * radius);
  }
  
  
  // math::vec3 sphere::proj(const math::vec3& ) const;
  // math::real sphere::dist(const math::vec3& ) const;

  math::vec3 sphere::normal(const math::vec3& v) const {
    assert(contains(v));

    return (v - center).normalized();
  }
  
}
