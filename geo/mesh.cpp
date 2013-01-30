#include "mesh.h"

#include <core/stl.h>

namespace geo {


  bool mesh::ok() const {
    return true;		// TODO !
  }


  geo::aabb mesh::aabb() const {
    geo::aabb res;
    vertices.each([&](math::natural i ) { res += vertices(i); });
    
    return res;
  }

  void mesh::clear() {
    vertices.resize( 0 );
    normals.resize( 0 );
    texcoords.resize( 0 );
    
    triangles.clear();
    quads.clear();

  }

}
