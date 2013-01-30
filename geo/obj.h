#ifndef GEO_OBJ_H
#define GEO_OBJ_H

#include <string>

namespace geo {
  
  struct mesh;
  
  namespace obj { 
    struct file;
    
    geo::mesh mesh(const file& );
    geo::mesh load(const std::string& );

  }
  
}

#endif
