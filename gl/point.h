#ifndef GL_POINT_H
#define GL_POINT_H

#include <gl/begin.h>
#include <gl/vertex.h>

namespace gl {
  
  template<class ... Vertex>
  void point(Vertex&& ... v ) {

    
    glBegin(GL_POINTS);
    vertex( v... );
    glEnd();
    
  }
}

#endif
