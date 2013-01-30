#include "screw_viewer.h"

#include <gl/frame.h>
#include <math/lie.h>
#include <math/se3.h>

#include <math/func/lie/subgroup.h>

#include <gl/init.h>
#include <gl/common.h>

namespace gui {

  
  void screw_viewer::init() {
    gl::init(); 

    target_ = new gui::frame;
    interp_ = new gui::frame;

    setMouseTracking(true);
    // setAxisIsDrawn( true );
    setSceneRadius(10);
    startAnimation();
  }


  screw_viewer::screw_viewer( QWidget* parent) 
    : alpha_viewer(parent) {

  }

  void screw_viewer::draw() {
    using namespace math;
    
    using math::euclid::operator*;
    
    math::lie::group<SE3> lie;
    
    auto fun = func::lie::subgroup< SE3 >(lie.log()(target_->transform() ) );

    interp_->transform( fun( alpha())  );
    
    target_->draw(1, true);
    interp_->draw(1, true);
    gl::frame(interp_->transform()).draw(lie.log()(target_->transform() ) );

    const math::natural traj_samples = 100;

    if( traj_samples ) {
      using namespace gl;
      disable(GL_LIGHTING );
      color( gl::white()) ;
      begin(GL_POINTS, [&] {
	  for(math::natural i = 0; i < traj_samples; ++i) {
	    vertex( fun( math::real(i) / math::real(traj_samples) ).translation() );	    
	  }
	});
      enable(GL_LIGHTING );
    }
    
  }


}
