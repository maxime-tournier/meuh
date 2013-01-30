#include "outline.h"

#include <gl/shader.h>
#include <gl/program.h>
#include <gl/error.h>
#include <gl/cull.h>
#include <gl/uniform.h>

#include <core/resource.h>


#include <iostream>

namespace gl {

  static gl::shader::fragment frag, ffog;
  static gl::shader::vertex vert;

  outline::outline(math::real width, 
		   const math::vec3& color, 
		   bool smooth) 
    : width(width), 
      color(color), 
      smooth(smooth) { }


  const program& outline::prog() const {
    return prog_;
  }

  program& outline::prog() {
    return prog_;
  }


  outline& outline::init() {
    
    using core::resource::ascii;
    if( prog().id() ) throw error("already init");
    
    vert.init("/main.vert");
    frag.init("/flat_color.frag");
    ffog.init("/fog.frag");
    
    prog().gen().attach( vert ).attach( ffog ).attach( frag ).link().report(std::cout);
    
    prog().enable().uniform("color") = color;
    prog().disable();

    return *this;
  }


  const outline& outline::enable() const {
    GLenum culled = cull::face();
    
    glPolygonMode (culled, GL_LINE);		// Draw Backfacing Polygons As Wireframes
    
    if(smooth) {
      glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);	// Use The Good Calculations
      gl::enable (GL_LINE_SMOOTH);			// Enable Anti-Aliasing
    }
    
    line_width ( width );			// Set The Line Width
	  
    cull::enable();
    cull::flip();
	  
    glDepthFunc (GL_LEQUAL);			// Change The Depth Mode
    return *this;
  }



  const outline& outline::disable() const {
    glDepthFunc (GL_LESS);				// Reset The Depth-Testing Mode
    
    cull::flip();
    cull::disable();
	  
    if(smooth) {
      gl::disable (GL_LINE_SMOOTH);			// Enable Anti-Aliasing
    }
    GLenum culled = cull::face();
    glPolygonMode (culled, GL_FILL);		// Draw Backfacing Polygons As Wireframes
    return *this;
  }

}
