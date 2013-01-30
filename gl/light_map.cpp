#include "light_map.h"

#include <math/vec.h>

#include <gl/gl.h>
#include <gl/enable.h>
#include <gl/texture.h>

#include <gl/shader.h>
#include <gl/program.h>
#include <gl/uniform.h>

#include <core/resource.h>

#include <vector>

#include <iostream>
#include <Eigen/LU>

namespace gl {

  
  namespace light_map {

    using namespace math;

    mat44 make_bias() {
      mat44 res;
      res.diagonal().head<3>().setConstant(0.5);
      res.col(3).head<3>().setConstant(0.5);
      res(3, 3) = 1;
      
      return res;
    }
    

    

    const mat44 bias = make_bias();
    
    void texture_matrix(const texture& map, 
			const mat44& cam_model, 
			const mat44& light_model_proj) {
      
    
      const mat44 tmp = // light.inverse() * cam.inverse();
	// light_proj * 
	bias * light_model_proj * cam_model.inverse();
      
      glMatrixMode(GL_TEXTURE);
      glActiveTexture( map.unit() );
       
      glLoadIdentity();	
      glLoadMatrixd(tmp.data());
        
      // Go back to normal matrix mode
      glMatrixMode(GL_MODELVIEW);
      
    }
    
  
  }  
}
