#include "patch_viewer.h"

#include <math/lie.h>
#include <math/se3.h>
#include <math/func/spline.h>
#include <gui/frame.h>
#include <gl/frame.h>

namespace gui {
  
  void patch_viewer::init() {
    
    for(unsigned int i = 0; i < 4; ++i) {
      nodes_.push_back( new gui::frame );
    }

    setMouseTracking(true);
    setAxisIsDrawn( false );
    startAnimation();
    setSceneRadius( 2 );
  }


  patch_viewer::patch_viewer( QWidget* parent) 
    : alpha_viewer( parent ) {
    
  }

  void patch_viewer::draw() {
    using namespace math;
    using math::euclid::operator*;
    
    const std::tuple<SE3, SE3, SE3, SE3> nodes =
      std::make_tuple( nodes_[0]->transform(),
		       nodes_[1]->transform(),
		       nodes_[2]->transform(),
		       nodes_[3]->transform() );
    
    using namespace func;
    spline::coeffs::catmull_rom<> catmull(0.0, 1.0);
    
    auto f = spline::make_patch( nodes, catmull);
    auto dt = math::body( alpha(), 1.0 );
    auto df = d(f)(dt);
    
    gl::frame(df.at()).draw(df.body());
    
    for(unsigned int i = 0; i < 4; ++i ){
      nodes_[i]->draw();
    }
    
  }


}
