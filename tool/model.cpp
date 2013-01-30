#include <sparse/matrix.h>

#include "model.h"

#include <gl/frame.h>
#include <gl/name.h>

#include <core/stl.h>
#include <core/unique.h>

#include <core/noop.h>

namespace tool {
  namespace model {


    void visual::draw(const phys::rigid::config& frames) const { 
      
      // TODO deal with gl names !
      core::each(meshes, [&](math::natural i, gl::mesh* m) {
	  gl::frame { frames(i) } ([&]{
	      m->draw();
	    });
	});
    }


    visual geometry::visual() const {
      model::visual res;

      core::each(meshes, [&](math::natural i, geo::mesh* m) {
	  res.meshes[i] = new gl::mesh(*m);
	});

      return res;
    }

    collision geometry::collision() const {
      model::collision res;
      
      core::each(meshes, [&](math::natural i, geo::mesh* m) {
	  auto p = new coll::vertices;
	  p->data = m->vertices;
	  res.primitives[i] = p;
	});
      
      return res;
    }

    collision::collision() : update(core::noop()) { }


  }
}


