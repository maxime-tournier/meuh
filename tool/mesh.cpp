#include "mesh.h"

#include <geo/mesh.h>
#include <geo/obj.h>
#include <geo/obj/file.h>

namespace tool {

  gl::mesh load_obj(const std::string& name) {
    gl::mesh res( geo::obj::mesh(  geo::obj::file( name ) ) );
    return std::move(res);
  }
  
  


}
