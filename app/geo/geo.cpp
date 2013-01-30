#include "geo.h"

#include <gl/init.h>
#include <gl/vbo.h>
#include <gl/sphere.h>
#include <gl/frame.h>

#include <gl/raii.h>
#include <gl/rigid.h>
#include <phys/unit.h>

std::unique_ptr<gl::vbo::data> sphere_data(  gl::sphere(2) );
gl::vbo sphere;

void viewer::init() {
  
  t = 0;
  dt = 0.001;

  sphere.gen().bind();
  sphere.send( *sphere_data );

  setAnimationPeriod(dt / phys::unit::ms); 
  startAnimation();

  
}

viewer::viewer() 
  : app(script().lua(), "app") {
  
  app
    .ref("dt", dt)
    .ref("time", t)
    .fun("reset", [&] { t = 0; } )
    ;
  
  script().exec("require 'geo'");
}

void viewer::draw() {

  { 
    gl::raii::matrix local;
    gl::rigid( target.transform());
    
    gl::scale( 0.1 );
    sphere.draw(GL_TRIANGLES);
  }
  
  arrow( math::vec3::Zero(), target.transform().translation(), 0.01);
  frame.draw(1, true);
  script().lua().string("hook(draw)");
} 


void viewer::animate() {
  using namespace math;
  vec3 omega = target.transform().translation();
  
  real theta = omega.norm();
  real alpha = t;
  if( theta > 1e-5) alpha = std::fmod(t * theta, 2*pi)/theta;
  
  frame.transform( SE3::rotation( Lie<SO3>().exp()( alpha *  omega)));
  t += dt;
  script().exec("hook(animate)");
}


int main(int argc, char** argv) {
  
  return tool::console_app(argc, argv, [&]{
      viewer* res = new viewer;
      
      return res;
    });

}


