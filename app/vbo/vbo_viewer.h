#ifndef VBO_VIEWER_H
#define VBO_VIEWER_H

#include <gl/mesh.h>


#include <QGLViewer/qglviewer.h>
#include <memory>


namespace gui {

  class vbo_viewer : public QGLViewer {

    gl::mesh mesh;
    
    Q_OBJECT

  public:

    vbo_viewer(QWidget* parent = 0 );

    void init();
    void draw();

  public slots:
    void load(const QString& );

  };
  


}

#endif
