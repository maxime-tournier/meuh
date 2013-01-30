#ifndef GL_FRAME_H
#define GL_FRAME_H


#include <math/se3.h>

#include <gl/translate.h>
#include <gl/matrix.h>
#include <gl/rotate.h>

namespace gl {
  
  // TODO make this a function
  struct frame {
    const math::SE3& data;

    frame(const math::SE3& data) : data( data ) { }

    template<class Action>
    const frame& operator()(const Action& action) const {
      matrix::safe( [&] {
	  translate( data.translation() );
	  rotate( data.rotation() );
	  action(); 
	} );
      return *this;
    }
    
    const frame& draw() const;
    const frame& draw(const math::twist& body_vel) const;
    
  };

  

}


#endif
