#include <gl/gl.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QGLViewer/qglviewer.h>

// #include <phys/skel/pack.h>
// #include <phys/skel/limits.h>
#include <gl/common.h>

#include <math/svd.h>
#include <math/quaternion.h>
#include <gl/sphere.h>
#include <gl/vbo.h>
#include <gl/init.h>
#include <gl/blend.h>

#include <core/log.h>
#include <math/func/coord.h>
#include <math/func/compose.h>

#include <tool/mocap.h>
#include <tool/angular_limits.h>

namespace gui {
  namespace viewer {



    struct limits : public QGLViewer {

      math::vector<math::SE3> data;
      math::lie::ellipsoid<math::SO3> ellipsoid;
      gl::vbo sphere;
      std::unique_ptr<gl::vbo::data> spheredata;

      void init() {
	gl::init();
	gl::enable( GL_NORMALIZE );
	  
	spheredata.reset(gl::sphere(5)); 
	sphere.gen().bind();
	sphere.send( *spheredata ).unbind();
	
      }

      void draw() {
	using namespace math;
	// draw points
	gl::disable(GL_LIGHTING);
	gl::color(gl::white());
	
	gl::begin(GL_POINTS, [&]() {
	    data.each([&](natural i ) {
		const real d = this->ellipsoid.dist2( this->data(i).rotation());

		// if( d > 1 ) std::cout << d << std::endl;
		
		gl::vertex( lie::log(this->ellipsoid.center.inverse() * this->data(i).rotation()) );
	      }); 
	  });
	gl::enable(GL_LIGHTING);
	
	using namespace math;

	const math::svd svd( ellipsoid.metric );
	math::mat33 R = svd.v();
	if( R.determinant() < 0 ) R.col(0) = -R.col(0);

	const math::quat q = R;
	
	gl::matrix::push();
	gl::rotate(q);
	// QGLViewer::drawAxis(1.0);

	// scale by two to get unit radius
	gl::scale(2.0 * svd.singular().array().sqrt().inverse());
	
	gl::blend{}([&]() {
	    gl::color( gl::transp(gl::white(), 0.3)) ;
	    sphere.draw(GL_TRIANGLES);
	  });
	
	gl::matrix::pop();
      };

      

    };


  }

}


int main(int argc, char** argv) {
  QApplication app( argc, argv );
  
  if( argc < 3 ) return -1;

  QWidget* wid = new QWidget;
  QVBoxLayout* lay = new QVBoxLayout;

  gui::viewer::limits viewer;
  
  const std::string filename = argv[1];
  const std::string joint = argv[2];

  const tool::mocap mocap(filename);
  
  const math::algo::stop inner(1e-14, 500);
  const math::algo::stop outer(1e-10, 10);
  
  using namespace math; 
  // const natural m = pack.mocap.clip.sampling.frames;
  // const natural n = pack.skeleton.size;

  const math::natural j = mocap.skeleton().find( joint );
  

  using func::operator<<;
  const auto data = mocap.clip.sampling.sample( mocap.relative() );
  
  // limits
  const auto limits = 
    tool::angular_limits{outer, inner, 0}( data, mocap.skeleton().root );
  
  // viewer setup
  viewer.data = mocap.clip.sampling.sample( func::coord<math::SE3>(j) 
					    << mocap.relative() );
  viewer.ellipsoid = limits.find(j)->second;
  
  assert( viewer.ellipsoid.metric.symmetric(1e-5) );

  core::log()("singular:", math::svd(viewer.ellipsoid.metric).singular().transpose());


  mocap.skeleton().each([&](math::natural i) {
      core::log()( mocap.skeleton().body(i).name, i);
    });

  
  lay->addWidget(&viewer);
  wid->setLayout( lay );
  
  wid->show();

  // weee
  return app.exec(); 
}
