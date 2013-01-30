#ifndef TOOL_MODEL_H
#define TOOL_MODEL_H

#include <map>
#include <memory>

#include <geo/mesh.h>
#include <gl/mesh.h>
#include <coll/primitive.h>

#include <phys/rigid/config.h>
#include <phys/object.h>

#include <coll/system.h>
#include <phys/system.h>

#include <core/callback.h>
#include <core/noop.h>

namespace tool {
  
  namespace model {

    struct visual {
      std::map<math::natural, gl::mesh* > meshes;
      
      void draw(const phys::rigid::config& ) const;
    };
    
    
    struct collision {
      
      std::map< math::natural, coll::primitive*> primitives;
      
      collision();
      
      core::callback update;
    };
    

    struct geometry {
      std::map<math::natural, geo::mesh* > meshes;

      model::visual visual() const;
      model::collision collision() const;
    };
 
    // TODO rename
    struct full {
      model::geometry geometry;
      model::visual visual;
      model::collision collision;
    };
    
  }

}



#endif
