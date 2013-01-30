#ifndef GUI_CONVERT_H
#define GUI_CONVERT_H

#include <math/types.h>
#include <QGLViewer/vec.h>

namespace gui {

  qglviewer::Vec convert(const math::vec3& x);
  math::vec3 convert(const qglviewer::Vec& x);


}


#endif
