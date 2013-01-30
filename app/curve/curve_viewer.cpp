#include "curve_viewer.h"

#include <math/lie.h>

#include <math/se3.h>
#include <math/func/interp.h>
#include <gl/frame.h>

#include <gl/common.h>
#include <gl/init.h>

#include <math/func/push.h>

using namespace math;

namespace gui {
  
  

  void curve_viewer::init() {
    gl::init(); 
    for(int i = 0; i < 4; ++i) {
      add_key();
    }
    
    setSceneRadius(10);
    setMouseTracking(true);
    startAnimation();
  }


  curve_viewer::curve_viewer( QWidget* parent) 
    : alpha_viewer( parent ) {

  }


  void curve_viewer::animate() {

      
  }

  void curve_viewer::draw() {
    // we draw interpolated before otherwise it wont let interact with
    // nodes
    for(unsigned int i = 0; i < nodes_.size(); ++i ){
      nodes_[i]->draw(0.6, true);
    }

    // TODO: better !
    auto n = nodes();
    assert( !n.empty() );

    auto f = func::curve(n);
    
    // auto df = func::d(f)(alpha())(1.0);
    auto at = f(alpha());
    
    using namespace gl;
    
    color( white() );
    gl::frame( at ).draw();
    // gl::frame( at ).draw( df );
    
    const math::natural traj_samples = 100;
    
    if( traj_samples ) {
      using namespace gl;
      disable(GL_LIGHTING );
      color( gl::white()) ;
      begin(GL_POINTS, [&] {
	  for(math::natural i = 0; i < traj_samples; ++i) {
	    vertex( f( math::real(i) / math::real(traj_samples) ).translation() );	    
	  }
	});
      enable(GL_LIGHTING );
    }

  }

  void curve_viewer::add_key() {
    nodes_.push_back(  new gui::frame()  );
  }
  
  curve_viewer::nodes_type curve_viewer::nodes() const{
    using namespace math;

    nodes_type res;
    assert( nodes_.size() );
    
    const double step =  length() / (nodes_.size() - 1);

    for(unsigned int i = 0; i < nodes_.size(); ++i) {
      res[ start() + i * step ] = nodes_[i]->transform();
    }
    
    return res;
  }

}
