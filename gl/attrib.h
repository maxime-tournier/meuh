#ifndef GL_ATTRIB_H
#define GL_ATTRIB_H

#include <math/types.h>

namespace gl {

  struct attrib {
    const GLint location;
    
    uniform& operator=(const math::vec3& );

  };



}


#endif
