#include "frame.h"


#include <QGLViewer/qglviewer.h>
#include <gl/enable.h>
#include <gl/color.h>
#include <gl/line.h>

namespace gl {

  const frame& frame::draw() const {
    return (*this)( []() { 
	QGLViewer::drawAxis();
      });
  }


  // TODO externalize in gl::twist
  const frame& frame::draw(const math::twist& body_vel) const {
    return (*this)( [&]() { 
	// QGLViewer::drawAxis();

	without(GL_LIGHTING, [&]() {
	    color(blue());
	    line( math::vec3::Zero(),  math::linear(body_vel) );
	    
	    color(red());
	    line( math::vec3::Zero(),  math::angular(body_vel) );
	  });
      });
  }


}
