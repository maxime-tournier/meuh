#ifndef GL_LIGHT_MAP_H
#define GL_LIGHT_MAP_H

#include <math/matrix.h>
#include <gl/gl.h>


namespace gl {

  struct texture;
  
  namespace light_map {

    // activate depht texture and set up texture matrix (cam ->
    // depthmap)
    void texture_matrix(const texture& map,
			const math::mat44& cam_model, 
			const math::mat44& light_modelproj);
  }
  
}


#endif
