#ifndef GL_LINE_H
#define GL_LINE_H

#include <gl/vertex.h>
#include <gl/begin.h>

namespace gl {
  
  // TODO lineS instead  ?
  template<class ... Vertices>
  void line(const Vertices&... vertices) {
    // ARGH lambdas and variadic templates have trouble beeing together
    // begin(GL_LINES)( [&] () {
    // 	vertex(vertices...);
    //   });
    
    glBegin(GL_LINES);
    vertex(vertices...);
    glEnd();
  }
  
}


#endif
