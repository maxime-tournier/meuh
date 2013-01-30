#ifndef TOOL_CONTACTS_H
#define TOOL_CONTACTS_H


#include <phys/dof.h>

#include <phys/constraint/unilateral.h>
#include <phys/constraint/bilateral.h>
#include <phys/contacts.h>

#include <phys/object.h>

#include <functional>
#include <vector>

#include <sparse/vector.h>
#include <sparse/matrix.h>
#include <map>

#include <coll/hit.h>
#include <memory>

#include <math/func/pull.h>
#include <math/func/ref.h>


namespace geo {
  namespace collision {
    struct primitive;
    struct hit;
  }
}


namespace tool {
  
  struct contacts : phys::contacts< coll::hit > {
    
    typedef phys::constraint::unilateral normal_type;
    std::vector< normal_type > normal;

    typedef phys::constraint::bilateral tangent_type;
    std::vector< tangent_type > tangent;

    typedef normal_type cone_type;
    std::vector<cone_type> cone;
    
  public:
    
    
    // TODO encapsulate ?
    struct {
      normal_type::matrix geometry;
      normal_type::vector corrections;
    } N;

    struct {
      tangent_type::matrix geometry;
      tangent_type::vector corrections;
    } T;

    struct {
      cone_type::matrix geometry;
      cone_type::vector corrections;
    } K;
    
    std::vector< math::vec3 > points;
    std::vector< math::mat > basis;

    struct zmp_type {
      math::vec3 base, dir;
    };
    
    zmp_type zmp(const std::vector<math::vec3>& f) const;
    
    std::vector<math::vec3> forces(const tangent_type::vector& lambda,
				   const normal_type::vector& mu) const;
    
    std::vector<math::vec3> forces_cone(const cone_type::vector& lambda) const;
    
    void update(const std::list< coll::hit >& hits);
    void update_cones(const std::list< coll::hit >& hits, math::natural n = 3);
    
    void clear();

  private:
    
  };
  
}



#endif
