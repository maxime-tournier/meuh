
#include <gl/gl.h>

#include <gui/vbo_viewer.h>
#include <gui/frame.h>
#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include <QObject>

int main(int argc, char** argv) {
  
  if( argc != 2 ) throw core::exception("i need a filename");
  
  QApplication app( argc, argv );
  
  QWidget* wid = new QWidget;
  QVBoxLayout* lay = new QVBoxLayout;

  gui::vbo_viewer* viewer = new gui::vbo_viewer( wid );
  viewer->load( argv[1] );
  
  lay->addWidget(viewer);
  
  wid->setLayout( lay );
  wid->show();

  return app.exec();
}
