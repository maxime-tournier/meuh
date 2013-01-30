#ifndef GEO_MESH_H
#define GEO_MESH_H

#include <geo/vertex.h>
#include <geo/normal.h>
#include <geo/texcoord.h>

#include <math/vec.h>
#include <geo/aabb.h>

namespace geo {


  // TODO add transformations
  struct mesh {
    
    math::vector< geo::vertex > vertices;
    math::vector< geo::normal > normals;
    math::vector< geo::texcoord > texcoords;

    template<int N>
    struct face {
      // we fill with -1 when not set
      math::natural vertex[N];
      math::natural normal[N];
      math::natural texcoord[N];
    };
    
    typedef face<3> triangle;
    typedef face<4> quad;
    
    std::vector< triangle > triangles;
    std::vector< quad > quads;

    bool ok() const;		// TODO (check consistency of normals, faces, etc)
    
    geo::aabb aabb() const;

    void clear();
  };

  
}


#endif
