#include "pga.h"

#include <QApplication>
#include <gl/init.h> 
#include <gl/frame.h> 

#include <script/console.h>
#include <core/log.h>


using namespace math;



viewer::viewer() 
  : app(script().lua(), "app"),   
    coords(vec3::Zero()),
    recording( false ),
    time(0),
    dt(0.01),
    play(recorded.end())
    
{
  
  app
    .fun("coord", [&](natural i) { return script::ref(coords(i)); })
    .fun("eigen", [&](natural i) { return script::ref(pga.eigen()(i)); })
    .fun("record", [this](bool v) { record(v); } )
    .fun("text", [this](const std::string& v) { text(v); } )
    .ref("dt", dt)
    .ref("time", time)
    .fun("mode", [&](natural i){
	using namespace gl;
	frame(f->transform())( [&]{
	    real scale = sceneRadius() * pga.eigen()(i) / pga.eigen()(0);
	    drawArrow( tool::convert(vec3::Zero()),
		       tool::convert(scale * pga.geodesics()(i)),
		       0.1);
	  });
	
      })
    .fun("play", [&] { core::log("playing"); play = recorded.begin(); })
    .fun("snapshot", [&](const std::string& where) { snapshot(where); } )
    .fun("snapshot_next", [&] { snapshot_next(); } )
    ;
  
  script().exec("require 'pga'");
}


void viewer::draw() {
  f->draw(10, true);
  
  // drawing shoud execute all in the same thread, therefore we don't
  // use exec
  engine.lua().string("hook(post_draw)");
}


void viewer::init() {
  gl::init();
  setMouseTracking(true);
  startAnimation();
  setAnimationPeriod(0);
  setSceneRadius( 10 );

  f = new gui::frame;
}

void viewer::animate() {
  
  if( recording ) {
    recorded.push_back(f->transform().rotation());
    play = recorded.end();
  } else {
    if( play != recorded.end() ) {
      f->transform( SE3::rotation(*play) );
      ++play;
    } else {
      if( !pga.eigen().empty() ) f->transform( SE3::rotation( pga.ccfk(3)( coords )) );
    }
  }

  engine.exec("hook(post_animation)");
  time += dt;
}


void viewer::record( bool value ) {
  stopAnimation();
  if( !value ) {
    core::log("stopped recording");
    compute();
    recording = false;
  } else {
    core::log("recording");
    recorded.clear();
    recording = true;
  }
  startAnimation();
}



void viewer::compute() {
  vector< SO3 > data;
  data.resize( recorded.size() );

  data.each([&](natural i) {
      data(i) = recorded[i];
    });
  
  pga.compute(data);
  core::log("eigen:", pga.eigen().transpose());
  
  coords = vec3::Zero();
}


int main(int argc , char** argv ) {
  QApplication app( argc, argv );
  
  viewer* wid = new viewer;
  wid->show();

  script::console console(wid->script());
  console.start();
  
  return app.exec();

}
