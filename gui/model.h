#ifndef GUI_MODEL_H
#define GUI_MODEL_H

#include <map>

#include <math/vec.h>
#include <gl/mesh.h>
#include <geo/mesh.h>

namespace gui {

  // TODO move this to tool:: ?
  class model {

    std::map<math::natural, gl::mesh* > _visual;

    typedef std::map<math::natural, geo::mesh > geometry_type;
    geometry_type _geometry;
    
  public:

    void add(math::natural i, const geo::mesh& mesh);
    void draw(math::vector< math::SE3 >& frames);
    
    const geometry_type& geometry() const;
    
    void clear();
  };


}



#endif
