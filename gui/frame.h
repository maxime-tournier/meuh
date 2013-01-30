#ifndef GUI_FRAME_H
#define GUI_FRAME_H


#include <math/se3.h>
#include <gl/gl.h>

#include <QGLViewer/manipulatedFrame.h>
#include <QGLViewer/keyFrameInterpolator.h>

#include <memory>
#include <gui/menu.h>
#include <gui/callback.h>

namespace gui {

  class frame : public qglviewer::ManipulatedFrame {
  public:
    
    frame();

    math::SE3 transform() const;
    void  transform(const math::SE3& g);
    virtual void draw(math::real scale = 1, bool force = false) const;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
  public:
    math::SE3 last;

    gui::menu menu;
    
    qglviewer::KeyFrameInterpolator interp;
    
    void release();

    // place frames here to delay finalization
    static std::list< std::unique_ptr<frame> > pending;

    gui::callback on_manipulated;
    
    // convenience
    math::vec3 pos() const;

  protected:
    void mouseReleaseEvent(QMouseEvent *const  	event,
			   qglviewer::Camera *const  	camera );

  };


}

#endif
