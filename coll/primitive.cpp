#include "primitive.h"

#include <core/stl.h>
#include <stdexcept>

#include <core/log.h>

namespace coll {
    
  primitive::primitive() : data(0) { }
  primitive::~primitive() {} 

  hit::list primitive::collide(const vertices& ) const {
    throw std::logic_error("vertices collision not implemented !");
  }

  hit::list primitive::collide(const plane& ) const {
    throw std::logic_error("plane collision not implemented !");
  }

  hit::list primitive::collide(const sphere& ) const {
    throw std::logic_error("sphere collision not implemented !");
  }
    
  hit::list vertices::collide(const plane& p) const {
    hit::list res;
      
    data.each( [&](math::natural i) {
	if(p.geo.collide(data(i))) {
	  math::real error;	// TODO actually compute error
	  res.push_back( hit{&p, this, data(i), p.geo.normal(), 0 } );
	}
      });
      
    return res;
  }
    
  hit::list vertices::collide(const vertices& ) const { return hit::list{}; }

  hit::list plane::collide(const vertices& p) const {
    return p.collide(*this);
  }

  hit::list plane::collide(const sphere& p) const {
    return p.collide(*this);
  }

  hit::list vertices::collide(const sphere& p) const {
    return p.collide(*this);
  }

        
  // double dispatch
  hit::list vertices::collide(const primitive& p) const {
    return p.collide(*this);
  }

  hit::list plane::collide(const primitive& p) const {
    return p.collide(*this);
  }

  hit::list sphere::collide(const primitive& p) const {
    return p.collide(*this);
  }
    

  // sphere::sphere(const ::geo::sphere& p) : geo(p) { }
    
  hit::list sphere::collide(const vertices& v) const {
    hit::list res;
      
    v.data.each([&](math::natural i) {
	if( geo.contains( v.data(i) ) ) {
	  math::real error;
	  res.push_back( hit{this, &v, v.data(i), geo.normal( v.data(i)), 0 } );
	}
      });
      
    return res;
  }


  hit::list sphere::collide(const plane& p) const {
    hit::list res;

    const math::vec3 n = p.geo.normal();
    const math::vec3 pos = geo.center - geo.radius * n;
      
    if(p.geo.collide(pos) ) {
      math::real error = -p.geo.dist(pos);
      res.push_back( hit{&p, this, pos, n, error} );
    }
      
    return res;
  }

  // sphere/sphere
  hit::list sphere::collide(const sphere& p) const {
    hit::list res;
      
    const math::vec3 diff = p.geo.center - geo.center;
    const math::real dist = diff.norm();
      
    // core::log("dist:", dist, "r1:", geo.radius, "r2:", p.geo.radius);
    const math::real error = p.geo.radius + geo.radius - dist;
    if( dist && (error >= 0) ) {
	
      math::real alpha = 0.5 * (geo.radius + dist - p.geo.radius);
      assert( alpha >= 0 );
	
      const math::vec3 n = diff / dist;
      const math::vec3 pos = geo.center + alpha * n;
	 
      res.push_back( hit{this, &p, pos, n, error} ); 

    }
      
    return res;
  }
    
}

  




