#ifndef GL_CYLINDER_H
#define GL_CYLINDER_H

#include <memory>
#include <math/types.h>
#include <gl/vbo.h>

namespace gl {

  
  class cylinder {
    
    struct part {
      struct {
	std::unique_ptr< vbo::data_type > vertices, indices;
      } data;
      
      struct {
	mutable gl::vbo vertices, indices;
      } vbo;
    } cap, side;
    
  public:
    cylinder(math::natural circ = 16, math::natural vert = 2);
    
    void init();
    void draw() const;

    
  };


}



#endif
