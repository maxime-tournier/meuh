#ifndef TOOL_GROUND_H
#define TOOL_GROUND_H

#include <gl/common.h>
#include <coll/primitive.h>

namespace tool {

  // TODO inherit ?
  struct ground {

    static void init(GLuint texture);
    void draw() const;
    coll::plane model;
    
  };
  
  
}


#endif
