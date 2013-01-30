#ifndef GL_OUTLINE_H
#define GL_OUTLINE_H

#include <gl/gl.h>
#include <gl/common.h>
#include <gl/blend.h>
#include <gl/program.h>

#include <gl/color.h>

namespace gl {

  
  class outline {
    typedef outline self;
    gl::program prog_;
  public:
    const math::real width;
    const math::vec3 color;
    const bool smooth;
    
    const program& prog() const;
    program& prog();
    
    self& init();

    outline(math::real width = 5, 
	    const math::vec3& color = math::vec3::Zero(), 
	    bool smooth = true);
    
    const self& enable() const;
    const self& disable() const;

    template<class Action>
    const self& draw( const Action& action ) const {
      enable();
      prog().draw( action );
      disable();
      return *this;
    }

    

  };

}

#endif
