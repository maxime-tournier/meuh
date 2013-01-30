#include "plane.h"

namespace geo {

  // collision is v.n <= -b
  bool plane::collide(const math::vec3& v) const {
    return (equation.head<3>().dot(v) + equation(3)) <= 0;
  }


  math::vec3 plane::normal() const { return equation.head<3>().normalized(); }
  
  math::real plane::dist(const math::vec3& v) const  {
    const math::real proj = equation.head<3>().dot(v);
    
    return (proj + equation(3)) / equation.head<3>().norm();
  }

  math::vec3 plane::origin() const { 
    return -equation(3) / equation.head<3>().squaredNorm() * equation.head<3>(); 
  }
  
  plane::plane() { }

  void plane::set(const math::vec3& origin, 
		  const math::vec3& normal) {
     equation << normal, -normal.dot(origin);
  }

  math::vec3 plane::proj(const math::vec3& v) const {
    math::vec3 o = origin();
    math::vec3 n = normal();

    math::vec3 diff = v - origin();

    diff -= n * n.dot(diff);
    
    return o + diff;
  }

}
