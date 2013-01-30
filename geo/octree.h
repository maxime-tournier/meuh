#ifndef GEO_OCTREE_H
#define GEO_OCTREE_H

#include <geo/aabb.h>

namespace geo {

  // TODO maybe this should move to coll ?
  struct octree {
    
    geo::aabb aabb;
    
    // could be long int
    typedef math::natural id_type;
    
    // a handle to the octree cell
    id_type id(const math::vec3& point) const;

    static constexpr math::natural max_depth = sizeof( id_type ) * 8 / 3;
    
    // its actual aabb TODO rename
    geo::aabb cell(id_type id, math::natural depth = max_depth) const;

    // an octree cell offset at a given depth
    struct offset_type {
      int x, y, z;
      math::natural depth;
    };

    // cell obtained by offsetting at a given depth
    static id_type neighbor(id_type id, offset_type off);
    
    // get offset between two cells at a given depth
    static offset_type offset(id_type src, id_type dest, math::natural depth = max_depth);
    
    // removes any significant bits after depth
    static id_type clamp(id_type id, math::natural depth = max_depth);

    // test
    static octree::id_type morton(id_type id);

  };


}


#endif
