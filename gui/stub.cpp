

#include <QGLViewer/qglviewer.h>
#include <math/matrix.h>


struct Viewer : QGLViewer {

  


  void draw();



};


int main(int argc, char** argv) {
  QApplication app( argc, argv );

  QWidget* wid = new QWidget;
  QVBoxLayout* lay = new QVBoxLayout;

  Viewer* viewer = new Viewer( wid );
  
  lay->addWidget(viewer);
  wid->setLayout( lay );
  
  wid->show();

  return app.exec();
}
