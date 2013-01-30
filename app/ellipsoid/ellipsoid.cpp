
#include <gl/frame.h>
#include <gl/sphere.h>
#include <gl/vbo.h>
#include <gl/init.h>
#include <gl/common.h>
#include <gl/blend.h>

#include <memory>

#include <gui/frame.h>

#include <QGLViewer/qglviewer.h>
#include <QApplication>
#include <QVBoxLayout>
#include <QSlider>

#include <core/log.h>

#include <math/matrix.h>
#include <math/svd.h>
#include <gui/alpha_viewer.h>

#include <tool/fix.h>
macro_pthread_fix;


using namespace math;

struct Viewer : gui::alpha_viewer {

  gl::vbo sphere_vbo;
  std::unique_ptr<gl::vbo::data> sphere_data;
 
  
  std::vector< gui::frame* > frame;
  math::vector< math::vec3 > scaling;
  
  Viewer(QWidget* parent) : gui::alpha_viewer(parent) { 

    
    frame.push_back( new gui::frame );
    frame.push_back( new gui::frame );
    
    scaling.resize(2);
    scaling(0) = vec3(1, 1.0/3.0, 1.0/4 );
    scaling(1) = vec3(1.5, 1, 1.0/5.0);
    

  }


  void draw(const mat44& A) {
    const mat33 M = A.topLeftCorner<3, 3>();
    
    math::svd svd(M, 0);
    
    mat33 R = svd.v();
    if( R.determinant() < 0 ) R.col(0) = -R.col(0);
    const vec3 sigma = svd.singular();
    const vec3 c = A.block<3, 1>(0, 3);

    const vec3 t = -svd.solve(c);
    
    gl::matrix::push();
    gl::translate(t);
    gl::rotate( quat(R) );
    gl::scale(2 * sigma.array().sqrt().inverse());
    sphere_vbo.draw(GL_TRIANGLES);
    gl::matrix::pop();
    
  } 

  static mat44 ellipsoid(gui::frame* f, const vec3& scaling) {
    const SE3 g = f->transform();
    
    const mat33 R  = g.rotation().matrix();
    const vec3 t  = g.translation();
    
    const mat33 M = R * scaling.array().square().matrix().asDiagonal() * R.transpose();
    const vec3 c = -M * t;

    // core::log log;
    // log("check:");
    // log(R);
    // log(svd(M).v());
    // log(svd(M).singular().transpose());

    mat44 res;

    res << 
      M, c, 
      c.transpose(), c.dot(t) - 1;
  
    return res;
  }

  void init() {
    gl::init();
    gl::enable( GL_NORMALIZE );

    sphere_data.reset( gl::sphere(2) );
    sphere_vbo.gen().bind();
    sphere_vbo.send( *sphere_data ).unbind();
    
    setMouseTracking(true);
    setSceneRadius(30); 
  }



  void draw() {
    auto axis = [](const vec3& scaling) -> vec3{
      return scaling.array().sqrt().inverse();
    };

    for(int i = 0; i < scaling.size(); ++i) {

      gl::frame{frame[i]->transform()}([&]() {
	  QGLViewer::drawAxis();
	});
      
      gl::color(gl::transp(gl::white()));
      gl::blend{}([&]() {
	  draw( ellipsoid(frame[i], scaling(i)) );
	});
      
      // gl::frame( frame[i]->transform() )(  [&]() {
      // 	  gl::scale(axis(scaling[i]) );
      // 	  sphere_vbo.draw(GL_TRIANGLES);
      // 	});
    }
    
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    const real x = 5 * (alpha() - 0.5);
    draw( x * ellipsoid( frame[0], scaling(0) ) +
    	  ellipsoid( frame[1], scaling(1) ) );
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  }



};


int main(int argc, char** argv) {
  QApplication app( argc, argv );

  QWidget* wid = new QWidget;
  QVBoxLayout* lay = new QVBoxLayout;

  int resolution = 1000;

  Viewer* viewer = new Viewer( wid );
  viewer->resolution( resolution );

  QSlider* slider = new QSlider ( Qt::Horizontal,  wid );
  slider->setMaximum( resolution );

  QObject::connect( slider, SIGNAL(valueChanged( int ) ),
		    viewer, SLOT( value( int ) ) );
  
  QObject::connect( slider, SIGNAL(valueChanged( int ) ),
		    viewer, SLOT( updateGL( ) ) );

  lay->addWidget(viewer);
  lay->addWidget(slider);
  wid->setLayout( lay );
  
  wid->show();

  return app.exec();
}
