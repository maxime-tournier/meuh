#ifndef COLL_PRIMITIVE_H
#define COLL_PRIMITIVE_H

#include <coll/hit.h>
#include <geo/plane.h>
#include <geo/sphere.h>

namespace coll {
    

  struct vertices;
  struct plane;
  struct sphere;
  
  // visitor/double dispatch hell
  struct primitive {
    virtual ~primitive();

    virtual hit::list collide(const vertices& ) const;
    virtual hit::list collide(const plane& ) const;
    virtual hit::list collide(const sphere& ) const;
    
    virtual hit::list collide(const primitive& ) const = 0;
      
    primitive();
    void * data;
  };


  struct vertices : primitive {
    math::vector<math::vec3> data;
      
    hit::list collide(const plane& p) const;
    hit::list collide(const vertices& p) const;
    hit::list collide(const sphere& p) const;

    hit::list collide(const primitive& p) const;
    
  };
    
    
  struct plane : primitive {
    ::geo::plane geo;

    hit::list collide(const vertices& p) const;
    hit::list collide(const primitive& p) const;
    hit::list collide(const sphere& p) const;
      
  };


  struct sphere : primitive {
    ::geo::sphere geo;
      
    hit::list collide(const vertices& p) const;
    hit::list collide(const plane& p) const;
    hit::list collide(const sphere& p) const;
      
    hit::list collide(const primitive& p) const;
  };


}



#endif
