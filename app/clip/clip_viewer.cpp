#include "clip_viewer.h"

#include <mocap/clip.h>
#include <gl/frame.h>
#include <gl/common.h>

#include <core/io.h>

#include <mocap/bvh.h>
#include <mocap/bvh/file.h>
#include <mocap/curves.h>

#include <math/func/compose.h>
#include <gl/skeleton.h>
#include <gl/init.h>

#include <math/func/ref.h>

#include <gui/viewer/fancy.h>
#include <core/stl.h>

#include <gl/blend.h>


#include <gui/viewer/text.h>

#include <phys/rigid/absolute.h>
#include <phys/rigid/relative.h>

namespace gui {
  
  clip_viewer::clip_viewer(QWidget* parent) : alpha_viewer(parent) { 
    ngeo = 10;
  }

  void clip_viewer::init() {


    gl::init();
    gl::skeleton::init();
    fancy::init(this, light.get());
    
    setSceneRadius(100);
    
    light.reset(new qglviewer::Camera);
    light->setPosition( qglviewer::Vec(0, 100, 0) );
   
    light->setSceneRadius( 40 );
    light->setZClippingCoefficient(2);
    light->setAspectRatio(1.0);
    setManipulatedFrame( light->frame() );
    
    startAnimation();
  }

 // std::vector<math::natural> to_remove(const phys::skel::pack& pack) {
 //    std::vector<math::natural> res;
    
 //    std::vector<std::string> names = { "rtoes", "ltoes", 
 // 				       "lfingers", "rfingers",
 // 				       "lthumb", "rthumb",
 // 				       "lhand", "rhand" };
    
 //    for(math::natural i = 0; i < names.size(); ++i) {
 //      const math::natural p = pack.find( names[i] )->get().id;
 //      res.push_back( p );
 //    }
    
 //    return res;
 //  }



  void clip_viewer::load( const QString& filename ) {
    
    
    
    // adaptor.reset( new phys::rigid::adaptor(pack->mocap.reroot.skeleton.get(), *pack->info) );
    // adaptor.reset( new phys::rigid::adaptor(pack->mocap.clip.skeleton.get(), bio::nasa(lengths) ) );
    mocap.reset( new tool::mocap(filename.toStdString()));
    
    anim.reset();
    anim.start( ::mocap::start( mocap->clip.data ) );
    anim.end( ::mocap::end( mocap->clip.data ) );
    // auto data = pack->mocap.clip.sampling.sample( pack->curves.joint );
    
    // using namespace math;
    // data.each([&](natural i) { 
    // 	// void relative translation
    // 	data(i).each([&](natural j) {
    // 	    data(i)(j) = SE3::rotation( data(i)(j).rotation() );
    // 	  });

    // 	if( i == 0 ) return;
	
    // 	// void badass joints variations
    // 	core::each( to_remove(*pack), [&](natural j) {
    // 	    data(i)(j) = data(0)(j);
    // 	  });

    // 	// void root variations
    // 	data(i)(0) = data(0)(0);
	
	
    //   });
    
    // pga.compute(data, ngeo, algo::stop().eps(1e-10).it(20), 
    // 		lie::of( data(0) ) );
    
    // std::cout << "geo : " << lie::of(pga.mean()).alg().coords(pga.geodesics()(0)).transpose() << std::endl;
  }

 

  
  void clip_viewer::draw() {

    if( mocap ) {
      
      auto dt = anim.dt();
      
      // auto joint = phys::skel::joint(pack->skeleton.topology)( dpose.at() );
      // joint(0) = math::SE3::identity();

      // auto projected = pga.map() ( pga.project( joint ) );
      // projected(0) = dpose.at()(0);

      // auto final = phys::skel::absolute(pack->skeleton.topology)( projected );
      
      // std::function<void ( void ) >  content = [&]() {
      // 	gl::skeleton( gl::skeleton::full ).draw_center( this->pack->skeleton.topology,
      // 							dpose.at() ) ;
      // };

      // auto lol = (*adaptor)( pack->mocap.reroot_global(t) );
      
      using math::func::operator<<;
      auto check = phys::rigid::absolute{mocap->skeleton()} << phys::rigid::relative{mocap->skeleton()};
      
      auto dpose = ( check << mocap->absolute() ).diff( dt );
      auto lol = dpose.at();
      
      const auto& topo = mocap->skeleton().topology;
      
      

      core::each(mocap->adaptor.index, [&](mocap::skeleton* j, math::natural i) {
	  gl::frame{lol(i)}( [&]() {
	      QGLViewer::drawAxis();

	      const math::vec3 offset = topo[i].dim.y() * math::vec3::UnitY() / 2;

	    });
	  gl::color( gl::white() );
	  viewer::text( topo[i].name.c_str() ).draw( this, lol(i).translation() );
	});

    
      gl::disable(GL_DEPTH_TEST );
      gl::disable(GL_LIGHTING);
      mocap->skeleton().prefix([&](math::natural i) {
	  using namespace boost;
	  
	  core::each(out_edges(i, topo ),
		     [&](phys::rigid::skeleton::topology_type::edge_descriptor e) {
		       
		       ::math::natural c = target(e, topo);
		       const phys::rigid::joint& j = topo[e];

		       ::math::vec3 start = (lol(i) * j.coords(i) ).translation(),
			   end = (lol(c) * j.coords(c)).translation(),
			   
			   parent = lol(i).translation(),
			   child = lol(c).translation();

		       using namespace gl;

		       point_size( 10 );
		       begin(GL_POINTS, [&]() {
			   
			   color(red());
			   vertex(start);

			 });
		       
		       point_size( 8 );
		       begin(GL_POINTS, [&]() {			  
			   color(green());
			   vertex(end );
			 });
		       
		       begin(GL_LINE_STRIP, [&]() {
			   color(red());
			   vertex(parent, start);

			   color(green());
			   vertex(end, child );
			   
			 });


		     });
	  
	});
      gl::enable(GL_LIGHTING);
      gl::enable(GL_DEPTH_TEST );

      // fancy::draw(camera(), light.get(), content );
      gl::color(gl::transp( gl::white(), 0.2 ));
      // gl::blend{}( content );
      gl::color(gl::white());

      
    }
  }  


}
