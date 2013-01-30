#include "aabb.h"

namespace geo {
  
  static const math::real inf = 1e32;


  void aabb::clear() {
    lower = math::vec3::Constant(inf);
    upper = math::vec3::Constant(-inf);
  }

  aabb::aabb() 
  
  { 
    clear();
  }

  bool aabb::empty() const {
    return (lower.array() >= upper.array()).any();
  }

  aabb& aabb::operator+=(const math::vec3& v) {
    v.each([&](math::natural i ) {
	if( v(i) < lower(i) ) lower(i) = v(i);
	if( v(i) > upper(i) ) upper(i) = v(i);
      });
    
    return *this;
  }

  aabb& aabb::operator+=(const aabb& other) {
    operator+=( other.lower );
    operator+=( other.upper );
    return *this;
  }


  math::vec3 aabb::center() const { return (lower + upper) / 2; }

  math::vec3 aabb::dim() const { return upper - lower; }

  math::vec3 aabb::radius() const { return dim() / 2; }
  


}
