#include "skeleton.h"

#include <gl/common.h>
#include <gl/sphere.h>
#include <gl/frame.h>
#include <gl/vbo.h>

#include <memory>

#include <QGLViewer/qglviewer.h>

#include <math/kinematics.h>
#include <gl/cylinder.h>
#include <phys/rigid/skeleton.h>

#include <core/debug.h>

#include <gl/cube.h>

namespace gl {

  namespace skel {

    static gl::vbo box;
    
    void draw(const phys::rigid::skeleton& skeleton,
	      const math::vector< math::SE3 >& absolute) {
      
      draw_box(skeleton, absolute);
      
    }

    void draw_box(const phys::rigid::skeleton& skeleton,
		  const math::vector< math::SE3 >& absolute) {
      
      skeleton.each([&](math::natural v) {
	  draw_box(skeleton, absolute, v);
	});
      
    }

    
    void draw_box(const phys::rigid::skeleton& skeleton,
		  const math::vector< math::SE3 >& absolute, 
		  math::natural v){
      using namespace gl;
      
      // upload vbo if needed
      if( !box.id()) box.send( gl::cube() );
      
    
      frame(absolute(v))([&] {
	  scale( skeleton.body(v).dim );
	      
	  if( v == skeleton.root ) {
	    glPushAttrib( GL_CURRENT_BIT );
	    // color( grey() );
	  } 
	  box.draw(GL_QUADS);

	  if( v == skeleton.root ) {
	    glPopAttrib();
	  }
	});
      
    };

  }

}


  // static vbo sphere_vbo;
  // static std::unique_ptr<vbo::data> sphere_data;
  
  // static gl::cylinder cylinder(20, 0);
  
  // static const math::real ratio = 1;

  // void skeleton::init() {
    
  //   sphere_data.reset( gl::sphere(2) );
  //   sphere_vbo.gen().bind();
  //   sphere_vbo.send( *sphere_data ).unbind();
    
  //   cylinder.init();
  // }
  
  

  // void skeleton::velocity(const math::T< math::SE3 >& dg ) const {
  //   const math::real alpha = 1.0;
  //   frame(dg.at()).draw( alpha * dg.body() );
  // }


  // void skeleton::reverse_velocity(const math::T< math::SE3 >& dg ) const {
  //   const math::real alpha = 1.0;
  //   frame(dg.at()).draw( -alpha * dg.body() );
  // }

  // // wireframe
  // void bone_wire(const math::vec3& start, 
  // 		 const math::vec3& end) {
  //   // color( white() );
  //   without(GL_LIGHTING, [&]() {
  // 	begin(GL_LINES,  [&]() { 
  // 	    vertex(start);
  // 	    vertex(end);
  // 	  } );
	
  // 	begin(GL_POINTS, [&]() {
  // 	    vertex(end );
  // 	  });
	
  //     });
    
  // }
  
  // void root_wire(const math::vec3& pos) {
  //   without(GL_LIGHTING, [&]() {
  // 	begin(GL_POINTS, [&]() {
  // 	    vertex( pos );
  // 	  });
  //     });
  // }
  

  // // wireframe
  // void bone_frames(const math::SE3& start, 
  // 		   const math::SE3& end) {
    
  //   without(GL_LIGHTING, [&]() {
  //   	begin(GL_LINES, [&]() { 
  //   	    vertex(start.translation());
  //   	    vertex(end.translation());
  //   	  } );
  //     } );
    
  //   gl::frame{end}( []() { QGLViewer::drawAxis(); } );
    
  // }
  
  // void root_frames(const math::SE3& pos) {
  //   gl::frame{pos}( []() { QGLViewer::drawAxis(); } );
  // }
  
  
  // const math::vec3& base_color = html(0xccbbff) ;
  
 
  // // full drawing
  // void bone_full(const math::SE3& start, const math::SE3& end) {
  //   assert( sphere_data );

  //   using namespace math;
    
  //   quat q;
  //   vec3 orig = start.translation();
  //   vec3 dir = end.translation() - start.translation();
  //   real length = dir.norm();
    
  //   q.setFromTwoVectors(vec3::UnitY(), dir);

  //   color( darker( base_color) );
    
  //   frame{end}( [&]() { scale( ratio ); sphere_vbo.draw(GL_TRIANGLES);  } );
    
  //   matrix::safe( [&]() {
  // 	translate( orig );
  // 	rotate( q );
	
  // 	translate(0, length /2, 0);
  // 	scale( 1, length, 1 );
  // 	color( base_color );
  // 	cylinder.draw();
  //     });
    
  // }

  // void phys_bone_full(const math::SE3& f, const math::real& length) {
  //   assert( sphere_data );
  //   using namespace math;
    
  //   frame{f}( [&]() { 
  // 	matrix::push();
  // 	translate( 0, length/2, 0);
  // 	sphere_vbo.draw(GL_TRIANGLES);
  // 	matrix::pop();
  // 	matrix::push();
  // 	translate( 0, -length/2, 0);
  // 	sphere_vbo.draw(GL_TRIANGLES);
  // 	matrix::pop();
  // 	scale( 1, length, 1 ); 
  // 	cylinder.draw();  } );
    
  //   // matrix::safe( [&]() {
  //   // 	translate( orig );
  //   // 	rotate( q );
    
  //   // 	translate(0, length /2, 0);
  //   // 	scale( 1, length, 1 );
  //   // 	color( base_color );
  //   // 	cylinder.draw();
  //   //   });
    
  // }
  


  
  // void root_full(const math::SE3& pos) {
  //   color( darker( base_color ) );
  //   frame{pos}( [&]() { scale(ratio); sphere_vbo.draw(GL_TRIANGLES);  } );
  // }

  // // dispatch
  // void skeleton::root(const math::SE3& pos) const {
  //   switch(how) {
  //   case wire: root_wire(pos.translation() ); break;
  //   case frames: root_frames( pos ); break;
  //   case full: root_full( pos ); break;
  //   }
  // }


  // void skeleton::bone(const math::SE3& start,
  // 		      const math::SE3& end) const {
  //   switch(how) {
  //   case wire: bone_wire(start.translation(), end.translation()); break;
  //   case frames: bone_frames( start, end ); break;
  //   case full: bone_full( start, end ); break;
  //   }
  // }
  

  // void skeleton::phys_bone(const math::SE3& frame,
  // 			   const math::real& length) const {
  //   switch(how) {
  //   // case wire: phys_bone_wire(start.translation(), end.translation()); break;
  //   // case frames: phys_bone_frames( start, end ); break;
  //   case full: phys_bone_full( frame, length ); break;
  //   default: throw gl::error("not implemented");
  //   }
  // }


