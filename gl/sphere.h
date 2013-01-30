#ifndef GL_SPHERE_H
#define GL_SPHERE_H

#include <gl/vbo.h>
#include <memory>

namespace gl {

  // TODO return std::unique_ptr ?  FIXME: cant, debug vbo first
  // (crashed work when data destroyed after sent)
  std::unique_ptr<vbo::data_type> sphere(math::real radius = 0.5, unsigned int n = 2 );
  
};


#endif
