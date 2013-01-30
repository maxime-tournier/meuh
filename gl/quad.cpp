#include "quad.h"
#include <gl/common.h>

namespace gl {
  
  void quad() { quad(GL_QUADS); }

  void quad(GLenum mode) { 
    
    gl::begin(mode, [&]() {
	const math::real w = 0.5;

	tex_coord(0, 0);
	normal(0, 0, 1);
	vertex(-w, -w);
	
	tex_coord(1, 0);
	normal(0, 0, 1);
	vertex(w, -w);

	tex_coord(1, 1);
	normal(0, 0, 1);
	vertex(w, w);

	tex_coord(0, 1);
	normal(0, 0, 1);
	vertex(-w, w);
      });
  }
  
}
