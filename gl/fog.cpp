#include "fog.h"

#include <gl/enable.h>

#include <math/vec.h>

namespace gl {
  namespace fog {

    using namespace math;

    void color(const vec3& v) {
      vector<GLfloat, 3> tmp = v.cast<float>();
      glFogfv(GL_FOG_COLOR, tmp.data() );
    }
    
    
    void density(math::real d) {
      float tmp = d;
      glFogf(GL_FOG_DENSITY, tmp);
    }
    
    void mode(GLenum m) {
      glFogi(GL_FOG_MODE, m);
    }

    void limits(real start, real end) {
      float tmp = start;
      glFogf(GL_FOG_START, tmp);

      tmp = end;
      glFogf(GL_FOG_END, tmp);
    }
    
    // convenience
    void enable() {
      gl::enable(GL_FOG);
    }

    void disable() {
      gl::disable(GL_FOG);
    }
    

  }
}
