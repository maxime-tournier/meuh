#include "frame.h"


#include <QMouseEvent>
#include <QMenu>

#include <QGLViewer/qglviewer.h>

#include <core/stl.h>
#include <gui/tweak.h>		// TODO move trigger outside tweak

#include <gl/sphere.h>
#include <gl/vbo.h>
#include <gl/color.h>
#include <gl/scale.h>
#include <gl/blend.h>

#include <gui/convert.h>

#include <core/unique.h>

#include <gui/fix.h>

namespace gui {

  static gl::vbo sphere_vbo;
  
  std::list< std::unique_ptr<frame> > frame::pending;

  void frame::draw(math::real scale, bool force ) const {

    if( !sphere_vbo.id() ) sphere_vbo.send( gl::sphere(0.5, 2) );
    
    using namespace gl;
    
    glPushMatrix();
    glMultMatrixd( matrix() );
  
    if( force ) {
      gl::color( gl::white() );
      QGLViewer::drawAxis(scale);
    }

    if( grabsMouse() ) {
      color( transp( orange() ) );
    } else {
      color( transp( red() ) );
    }
    
    
    gl::scale(0.1 * scale);
    
    blend()( [&] {
	sphere_vbo.draw(GL_TRIANGLES);
      });
    
    glPopMatrix();
  }


  math::vec3 frame::pos() const { 
    return gui::convert( position() );
  }

  frame::frame() 
    : qglviewer::ManipulatedFrame(),
      interp(this)
  {
      setGrabsMouse(true);

      connect(this, SIGNAL(manipulated()),
	      &on_manipulated, SLOT(trigger()));
      
  };

  math::SE3 frame::transform() const {
    using namespace math;

    const qglviewer::Vec& t = position();
    const qglviewer::Quaternion& q = orientation();
    
    
    const vec3 trans(t[0], t[1], t[2] );
    const quat rot( q[3], q[0], q[1], q[2] ); 
    
    return SE3(trans, rot); 
  }


  void  frame::transform(const math::SE3& g) {
    using namespace math;
    
    const vec3& trans = g.translation();
    const quat& rot = g.rotation().quaternion();
    
    const qglviewer::Vec t( trans.x(), trans.y(), trans.z() );
    const qglviewer::Quaternion q( rot.x(), rot.y(), rot.z(), rot.w() );
    
    setPosition( t );
    setOrientation( q );
  }


  void frame::mouseReleaseEvent(QMouseEvent* e,
			      qglviewer::Camera* cam) {

    qglviewer::ManipulatedFrame::mouseReleaseEvent(e, cam);
    
    if ((e->button() == Qt::RightButton) && (e->modifiers() == Qt::ShiftModifier)) {
      menu.popup(e->globalPos());
    } 
    
    
  }

  void frame::release() {
    setGrabsMouse(false);
    removeFromMouseGrabberPool();	
    pending.push_back( core::unique(this) );
  }


}
