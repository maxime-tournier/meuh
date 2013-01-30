#ifndef PHYS_CONTACTS_H
#define PHYS_CONTACTS_H

#include <functional>
#include <math/types.h>

#include <phys/object.h>
#include <sparse/matrix.h>
#include <map>

namespace phys {

  // attempting to provide a contacts manager basis
  template<class Hit>
  class contacts {

  protected:
    
    typedef sparse::mat_chunk::RowXpr row_type;

    // fills a matrix row given a force at contact point
    typedef std::function< void(row_type, const math::vec3& ) > pullback_type;
    
    // builds a pullback given a hit
    typedef std::function< pullback_type ( const Hit& hit ) > mapper_type;
    
    // TODO unordered map ?
    std::map< phys::dof::key, mapper_type > mapper;
    
  public:

    // register a dof and its contact kinematics. 
    // @f should return the forward kinematics given a hit
    template<class G, class F>
    void map(const phys::object<G>* obj, const F& f) {
      
      mapper[obj] = [&, obj, f](const Hit& hit) -> pullback_type {
	
	// direct kinematics
	const auto kin = f( hit );
	
	using namespace math::func;
	
	// pullback at current conf
	const auto dTkin = dT( ref(kin) )( obj->g() );
	
	return [dTkin](row_type j, const math::vec3& f) {
	  const auto force = dTkin( f.transpose() );
	  math::euclid::get(j.transpose(), force);
	};
	
      };
    }

    void clear() { mapper.clear(); }

  };



}



#endif
