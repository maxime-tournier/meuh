#ifndef GL_REFLECT_H
#define GL_REFLECT_H


#include <math/se3.h>

#include <gl/matrix.h>
#include <gl/frame.h>
#include <gl/scale.h>
#include <gl/get.h>
#include <gl/enable.h>
#include <gl/cull.h>

namespace gl {
  
  struct reflect {

    // reflection plane frame (normal: y)
    const math::SE3 frame;
    reflect(const math::SE3& = math::SE3::identity());
    
    template<class Content>
    void operator()(const Content& action) {
      matrix::safe( [&]() {
	  scale(1, -1, 1);
	  
	  cull::flip();
	  action();
	  cull::flip();
	  
	});
      
    }
    
  };
  
  
  
  
}



#endif
