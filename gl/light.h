#ifndef GL_LIGHT_H
#define GL_LIGHT_H

#include <gl/gl.h>
#include <math/types.h>

namespace gl {
  
  class light {
    GLenum id;
  public:
    
    light(GLenum );

    const light& enable() const; 
    const light& disable() const; 
    
    
    const light& ambient(const math::vec3& ) const;
    const light& diffuse(const math::vec3& ) const;
    const light& specular(const math::vec3& ) const;
    const light& position(const math::vec3& ) const;
    const light& direction(const math::vec3& ) const;

    static light get(math::natural i);    

    struct model {
      static void ambient(const math::vec3& );
    };
    
  };



}


#endif
