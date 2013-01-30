#include "model.h"

#include <gl/frame.h>
#include <gl/name.h>

#include <core/stl.h>

namespace gui {


  void model::add(math::natural i, const geo::mesh& mesh) {
    _geometry[i] = mesh;
    _visual[i] = new gl::mesh( mesh );
  }


  void model::clear() { 
    _geometry.clear();
    
    core::each(_visual, [&](math::natural, gl::mesh* m) {
	delete m;
      });

    _visual.clear();

  }

  void model::draw(math::vector< math::SE3 >& frames) { 
    using namespace gl;

    core::each(_visual, [&](math::natural i, mesh* m) {
	frame { frames(i) } ([&]() {
	    name::push(i);
	    m->draw();
	    name::pop();
	  });
      });
  }


  const model::geometry_type& model::geometry() const { return _geometry; }

}
