#include "traj.h"

#include <QApplication>

#include <math/func/coord.h>
#include <math/func/SE3.h>
#include <tool/mocap.h>
#include <core/stl.h>


#include <script/console.h>
#include <core/raii.h>

#include <gl/init.h>
#include <core/unique.h>

#include <core/string.h>

using namespace math;




void viewer::draw() {
  using namespace gl;

  disable(GL_LIGHTING);
  point_size(2);

  // begin(GL_POINTS, [&]{


  //     color(green());
  //     vertex(curve.data.begin()->second);

  //     color(blue());
  //     vertex(curve.data.rbegin()->second);

  //     core::each(curve.data, [&](real t, const vec3& value) {
  // 	  color(white());
  // 	  vertex(value);
	  
  // 	});
  //   });
  
  color(red());
  picker.draw();
  

  enable(GL_LIGHTING);
  
  mutex.lock();
  core::each( frames, [](gui::frame* f) { f->draw(); } );
  mutex.unlock();
}



void viewer::init() { 
  gl::init();
  setSceneRadius(100);
  
  app.ref("level", curve.level);
  script().exec("require 'traj'");
  
  setSelectRegionWidth(8);
  setSelectRegionHeight(8);
  setMouseTracking( true );

  // TODO maybe a bit less
  core::each(curve.data(), [&](math::real t, const math::vec3& value) {

      tool::pickable pick;

      pick.draw = [this,t] {       
	using namespace gl;
	point_size(8);
	begin(GL_POINTS, [&] {
	    vertex( curve(t) );
	  });
	point_size(1);
      };
      
      pick.pick = [this, t] (const math::vec3& ) {
	curve.gui_edit(frames, t, 5);
      };
      
      picker.add( pick );
      
    });
  
  
  startAnimation();
}

viewer::viewer() 
  : app(script().lua(), "app")
{

}



int main(int argc , char** argv ) {
  QApplication app( argc, argv );
  
  viewer* wid = new viewer;

  if( argc >=2 ){
    tool::mocap mocap(argv[1]);
    
    int lhand = mocap.skeleton().find("lhand");

    auto fun = func::translation<>() << func::coord<SE3>(lhand) << mocap.absolute();
    auto data = mocap.clip.sampling.sample( fun );

    std::map<math::real, math::vec3> map;
    data.each([&](natural i) {
	map[ mocap.clip.sampling.time(i) ] = data(i);
      });
      
    wid->curve.data( map );
    
    // script::console console(wid->script());
    // auto start = core::raii::start(console);
    
    
    wid->show();
    return app.exec();
  }
}
 
