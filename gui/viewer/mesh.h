#ifndef GUI_VIEWER_MESH_H
#define GUI_VIEWER_MESH_H

#include <gl/mesh.h>
#include <QGLViewer/qglviewer.h>

namespace gui {
  namespace viewer {

    class mesh : public QGLViewer {
      Q_OBJECT

      std::unique_ptr< gl::mesh > mesh_;
      public:
      
      void draw();
      void init();
	

      static int run(int, char**);

    public slots:
      
      void load(const QString& );
      
    };

    


  }
}


#endif
