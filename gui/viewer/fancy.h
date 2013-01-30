#ifndef GUI_VIEWER_FANCY_H
#define GUI_VIEWER_FANCY_H


#include <gl/gl.h>

#include <QGLViewer/qglviewer.h>
#include <gui/frame.h>

#include <vector>
#include <memory>

#include <mocap/ik/handle.h>

#include <gl/texture.h>


namespace gui {

  namespace fancy {
  
    void init(QGLViewer* wid, qglviewer::Camera* light);

    void cast(qglviewer::Camera* camera,
	      qglviewer::Camera* light,
	      const std::function< void ( void ) >& content );

    void draw(qglviewer::Camera* camera, 
	      qglviewer::Camera* light,
	      const std::function< void (void ) >& content );
  }

}

#endif
