#include "nasa.h"
#include <core/error.h>

#include <phys/unit.h>

#include <core/unused.h>
#include <core/find.h>
#include <core/stl.h>

#include <math/vec.h>
#include <math/matrix.h>

#include <phys/rigid/inertia.h>



namespace tool {
  namespace bio {

    using phys::unit::g;

    nasa::real_map nasa::make_mass() const {
      real_map masses;

      for( const part* p = &head; p <= &foot; ++p) {
	using namespace core;
      
	// weights total
	math::real total = 0;
      
	core::each( p->weights, [&]( const std::string& name , math::real w) {
	    //  we only sum for names in _dim
	    if( _dim.find(name) != _dim.end() ) total += w;
	  });
	  
	  
	// add contribution 
	core::each(p->weights, [&]( const std::string& node, math::real weight) {

	    if( masses.find(node) == masses.end() ) masses[ node ] = 0.0;
	    masses.find(node)->second += p->mass * weight / total;
	  
	  });


      }

      return masses;
    }


    bool nasa::part::contains(const std::string& name) const { return weights.find(name) != weights.end(); }

    nasa::vec3_map nasa::make_dim(const real_map& lengths) const {
      vec3_map res;
    
      using namespace math;

      core::each(lengths, [&](const std::string& name, real length) {
	
	  vec3 ratio(1.0 / 4.0, 1.0, 1.0 / 4.0);
	
	  if( foot.contains(name)  ) {
	    ratio = vec3(1.0, 1.0, 1.0 / 4.0);
	  } else if( hand.contains(name) ) {
	    ratio = vec3(2.0, 1.0, 2.0);
	  } else if( forearm.contains(name)  ) {
	    ratio = vec3(1.0/3.0, 1.0, 1.0/3.0);
	  } else if( upperarm.contains(name)  ) {
	    ratio = vec3(1.0 / 5.0, 1.0, 1.0 / 5.0);
	  }
	
	  res[name] = length * ratio;
	
	});

      return res;
    }
    


    static std::map<std::string, math::real> weights() { return {}; }

    template<class ... Args>
    static std::map<std::string, math::real> weights(const std::string& name, math::real weight,
						     Args&&... tail) {
      auto res = weights(std::forward<Args>(tail)...);
      res[name] = weight;

      return res;
    }


  
    nasa::part::part(const math::real mass, const std::map<std::string, math::real>& weights)
      : mass(mass), weights(weights) { }
  
  
    nasa::nasa(const std::map< std::string, math::real >& dim) 
      : 	head( 4400 * g,   weights("head", 1,
					  "head end", 1) ),
	
		neck( 1100 * g,   weights("upperneck", 1,
					  "lowerneck", 1) ),
	
		thorax(26110 * g, weights("thorax", 1,
					  "lclavicle", 1,
					  "rclavicle", 1) ),

		abdomen(2500 * g, weights("upperback", 1,
					  "lowerback", 1)),

		pelvis(12300 * g, weights("lhipjoint", 1,
					  "rhipjoint", 1)),
  
		upperarm(2 * 2500 * g, weights("lhumerus", 1,
					       "rhumerus", 1)),
	
		forearm( 2 * 1450 * g, weights("lradius", 1,
					       "rradius", 1)),
	
		hand( 2 * 530 * g , weights("lhand", 2,
					    "rhand", 2,
					    "lfingers", 1,
					    "rfingers", 1,
					    "lthumb", 1,
					    "rthumb", 1)),
	
		hip( 2 * 3640 * g, weights("lhipjoint", 1,
					   "rhipjoint", 1)),
	      
		thigh(2 * 6700 * g, weights( "lfemur", 1,
					     "rfemur", 1) ),

		calf( 2 * 4040 * g, weights( "ltibia", 1,
					     "rtibia", 1) ),
	
		foot( 2 * 1010 * g, weights( "lfoot", 2,
					     "rfoot", 2,
					     "ltoes", 1,
					     "rtoes", 1)),

		_dim(make_dim(dim) ),
		_mass(make_mass())
	
    {
    
    
    }

  
  
    math::real nasa::mass( const std::string& name ) const {
      return core::find(_mass, name)->second;
    }

    math::vec3 nasa::dim( const std::string& name ) const {
      return core::find(_dim, name)->second;
    }
  
    math::vec3 nasa::inertia( const std::string& name ) const {
      return mass(name) * phys::rigid::inertia::ellipsoid( dim(name) );
    }
    
  }
}
