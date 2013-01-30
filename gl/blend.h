#ifndef GL_BLEND_H
#define GL_BLEND_H

#include <gl/gl.h>

namespace gl {


  struct blend {
    const GLenum sfactor;
    const GLenum dfactor;

    blend(GLenum sfactor = GL_SRC_ALPHA,
	  GLenum dfactor = GL_ONE_MINUS_SRC_ALPHA);

    template<class Action>
    void operator()(const Action& action) const {
      glEnable(GL_BLEND);
      glBlendFunc(sfactor, dfactor);
      action();
      glDisable(GL_BLEND);

    }

  };

}




#endif
