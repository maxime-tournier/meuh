
#include <plugin/kinect.h>
#include <plugin/opencv.h>
#include <plugin/load.h>

#include <core/share.h>

#include <script/console.h>
#include <tool/viewer.h>

#include <script/meuh.h>

#include <QApplication>

#include <core/log.h>
#include <thread>

struct viewer : tool::viewer {

  plugin::kinect* kinect;
  plugin::opencv* opencv;
  
  plugin::kinect::rgb_map rgb;
  plugin::kinect::ir_map ir;
  plugin::kinect::pcl_map pcl;
  
  viewer( plugin::kinect* kinect,
	  plugin::opencv* opencv ) 
    : kinect( kinect),
      opencv( opencv )
  { 
  
     
    kinect->start_pcl([&](const plugin::kinect::pcl_map& map,
			  plugin::kinect::timestamp ) {
			pcl = map;
		      });
    
    kinect->start_rgb( [&](const plugin::kinect::rgb_view& map,
    			   plugin::kinect::timestamp ) {
    			 rgb = map;
    		       });
    
    // kinect->start_ir( [&](const plugin::kinect::ir_map& map,
    // 			   plugin::kinect::timestamp ) {
    // 			ir = map;
    // 		      });
    
    // plugin::opencv::rgb_image img;
    // opencv->load(img, core::share("/texture/serious.png"));
    // rgb = img;
  }

  void init() {
    startAnimation();
    setSceneRadius( 3 );	// 3 meters

    camera()->setPosition( -camera()->position() );
    camera()->lookAt( sceneCenter() );
  }
  
  void draw() {
    
    using namespace gl;
    
    disable(GL_LIGHTING);

    using namespace math;

    
    // real min_dist = -10;
    // real scale = 0.0021;

    // real w = 640;
    // real h = 480; 

    // real tmp = 0.05;

    point_size( 4.0 );
    begin(GL_POINTS, [&] {

	pcl.each([&](math::natural i, math::natural j) {
	    
	    // vec3 c = ir(i, j) * vec3::Ones();
	    vec3 c = rgb(i, j).gl();
	    vec3 v = pcl(i, j);

	    if( v.z() > 0 ) {
	      color( c );
	      vertex( v );
	    }
	  });
	
      });

    enable(GL_LIGHTING);
  }

  ~viewer() {
    kinect->close();
  }

};

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  
  auto kinect = plugin::load< plugin::kinect >();
  auto opencv = plugin::load< plugin::opencv >();

  auto c = kinect->dev_count();
  core::log("devices:",  c);
  
  if( c == 0 ) {
    core::log("no device, exiting");
    return -1;
  } 

  kinect->open( 0 );
  
  viewer* wid = new viewer( kinect.get(), opencv.get() );
  
  wid->show();
  
  script::api api("kinect");

  plugin::kinect::rgb_map rgb;
  image::gray ir;

  api
    .fun("tilt", [&](double d) {
      kinect->tilt( d );
    })
    .fun("rgb", [&] {

	kinect->start_rgb([&](const plugin::kinect::rgb_view& img, core::unused) {
	    rgb = img;
	  });
	
      })
    .fun("ir", [&] {
	
	kinect->start_ir([&](const plugin::kinect::ir_map& img, core::unused) {
	    ir = img;
	  });
	
      })
    
    .fun("save", [&] {
	{
	  image::rgb img;
	  img.resize( rgb.rows(), rgb.cols());
	  
	  img.each( [&](math::natural i, math::natural j) {
	      img(i, j) = rgb(i, j).gl();
	    });
	  
	  opencv->save("/tmp/rgb.png", img);
	}
	opencv->save("/tmp/ir.png", ir);
      });
  ;
  
  script::require("kinect");

  std::thread t{ script::console{} };
  
  return app.exec();
  
}
