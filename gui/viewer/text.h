#ifndef GUI_VIEWER_TEXT_H
#define GUI_VIEWER_TEXT_H

#include <gl/gl.h>
#include <math/vec.h>
#include <QString>
#include <QFont>


class QGLWidget;
class QGLViewer;

namespace qglviewer{
  struct Camera;
}


namespace gui {

  namespace viewer {

   
    class text {
      const QString string;
      const QFont font;
    public:
      text(const QString& string,
	   const QFont& font = QFont());

      void draw(QGLWidget*) const;
      void draw(QGLViewer* , const math::vec3& center) const; // center in world frame
    };
    

  }

}


#endif
