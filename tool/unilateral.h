#ifndef TOOL_UNILATERAL_H
#define TOOL_UNILATERAL_H

#include <phys/dof.h>

#include <phys/constraint/unilateral.h>
#include <phys/constraint/bilateral.h>

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

  class unilateral {

    typedef phys::constraint::unilateral constraint_type;
    std::vector< constraint_type > constraints;
    
    typedef sparse::mat_chunk::RowXpr row_type;
    
     // fills a row given force
    typedef std::function< void(row_type, const math::vec3& ) > pullback_type;

    // gives the pullback given hit at pos TODO give hit instead ?
    typedef std::function< pullback_type( const math::vec3& pos,
					  const coll::primitive* prim) > mapper_type;
    
    std::map< phys::dof::key, mapper_type > mapper;
    
  public:
    
    // @f returns the forward kinematics given a world position
    template<class G, class F>
    void map(const phys::object<G>* obj, const F& f) {
      
      mapper[obj] = [&, obj, f](const math::vec3& pos,
				const coll::primitive* prim) -> pullback_type {
	
	// direct kinematics
	const auto kin = f( pos, prim );
	
	using namespace math::func;

	// pullback at current conf
	const auto dTkin = dT( ref(kin) )( obj->g() );
	
	return [dTkin](row_type j, const math::vec3& f) {
	  const auto force = dTkin( f.transpose() );
	  math::euclid::get(j.transpose(), force);
	};
	
      };
    }
    
    // TODO encapsulate ?
    constraint_type::matrix geometry;
    constraint_type::vector corrections;
    
    void update(const std::list< coll::hit >& hits);

    void clear();
  };

}


#endif
