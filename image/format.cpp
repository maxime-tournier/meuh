#include "format.h"

namespace image {
  namespace format {

    rgb::rgb() { }
    
    rgb::rgb(const math::vec3& c) 
      : r( c.x() * 256 ),
	g( c.y() * 256 ),
	b( c.z() * 256 ) { }      

    math::vec3 rgb::gl() const {
      return { r / 256.0, g/256.0, b/256.0 };
    }

    bgr::bgr() { }
    
    bgr::bgr(const math::vec3& c) 
      :
      b( c.z() * 256 ),
      g( c.y() * 256 ),
      r( c.x() * 256 )
    { }      
    
    math::vec3 bgr::gl() const {
      return { r / 256.0, g/256.0, b/256.0 };
    }
    
    
  }
}
