#include "sphere.h"

#include <QApplication>
#include <gl/init.h>

#include <math/random.h>
#include <gl/sphere.h>
#include <gl/blend.h>
#include <gl/frame.h>

#include <math/lie/pga.h>
#include <math/quat.h>
#include <math/se3.h>

#include <math/vec.h>
#include <math/func/vector.h>

gl::vbo sphere;

void viewer::init() {
  gl::init();

  sphere.gen().bind();

  gl::vbo::data* data = gl::sphere(4);
  sphere.send( *data );
  
}


void viewer::draw() {
  if(on_draw) on_draw();
}


using namespace math;

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  
  viewer* wid = new viewer;

  auto gen = math::random<vec3>::uniform(-0.7, 0.7);

  vec3 base = vec3::UnitY();

  natural n = 200;
  vec3 scale(1, 0.5, 0.5);
  
  vector<vec3> samples;
  samples.resize(n);

  samples.each([&](natural i) {
      samples(i) = 0.5 * (base + scale.cwiseProduct(gen())).normalized();
    });
  
  lie::pga<vec3> pga(samples);
  
  quat q1, q2; q1.setFromTwoVectors(vec3::UnitX(), pga.geodesics()(0) );
  
  q2.setFromTwoVectors(vec3::UnitY(), q1.inverse()*pga.geodesics()(1));

  SE3 g(pga.mean(), SO3(q1 * q2));
  
  wid->on_draw = [&] {
    

    using namespace gl;
    color( white());
   
    point_size(3);

    samples.each([&](natural i) {
	gl::matrix::push();
	translate( samples(i) );
	gl::scale(0.005);
	::sphere.draw(GL_TRIANGLES);
	gl::matrix::pop();
      });

    // begin(GL_POINTS, [&]{
    // 	samples.each([&](natural i) {
    // 	    normal( samples(i).normalized());
    // 	    vertex( samples(i));
    // 	  });
    //   });
    

    
    real radius = 0.01;
    real scale = 0.5 / pga.eigen()(0);
    

    bool show_pca = false;

    if( show_pca ) {
      // PCA 
      frame{g}([&]{
	  color( red() );
	  wid->arrow(vec3::Zero(), scale * pga.eigen().cwiseProduct(vec3::UnitX())  , radius);
	
	  color( green() );
	  wid->arrow(vec3::Zero(), scale * pga.eigen().cwiseProduct(vec3::UnitY()) , radius);

	  color( blue() );
	  wid->arrow(vec3::Zero(), scale * pga.eigen().cwiseProduct(vec3::UnitZ()) , radius);
	});
    }

    bool show_pga = true;
    

    if( show_pga ) {
      // PGA
      vec3 mean = pga.mean().normalized();

      natural n_steps = 100;
      line_width(3.0);

      color( red() );
      begin(GL_LINE_STRIP, [&] {
	  for(natural i = 0; i < n_steps; ++i) {
	    vertex(0.5 * (mean + (i * pga.eigen()(0)/pga.eigen()(0)  * pga.geodesics()(0)) / n_steps).normalized());
	  }
	});
    
      color( green() );
      begin(GL_LINE_STRIP, [&] {
	  for(natural i = 0; i < n_steps; ++i) {
	    vertex(0.5 * (mean + (i * pga.eigen()(1)/pga.eigen()(0) * pga.geodesics()(1)) / n_steps).normalized());
	  }
	});
    }    

    blend()([&]{
	color(transp(white(), 0.4));
	::sphere.draw(GL_TRIANGLES);
      });
    
  };

  

  wid->show();  
  return app.exec();
}
