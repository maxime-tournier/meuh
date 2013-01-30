#ifndef GL_FOG_H
#define GL_FOG_H

#include <math/types.h>
#include <gl/gl.h>

namespace gl {

  namespace fog {

    void color(const math::vec3&);
    
    void density(math::real d);
    
    void mode(GLenum m);
    
    void limits(math::real start, math::real end);

    // convenience
    void enable();
    void disable();

  }
  
}



#endif
