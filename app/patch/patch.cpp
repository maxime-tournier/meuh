#include <gl/gl.h>
#include <gui/frame.h>
#include "patch_viewer.h"

#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include <QObject>


#include <tool/fix.h>
macro_pthread_fix;


int main(int argc, char** argv) {
  QApplication app( argc, argv );

  QWidget* wid = new QWidget;
  QVBoxLayout* lay = new QVBoxLayout;

  int resolution = 1000;
  
  // gui::screw_viewer* viewer = new gui::screw_viewer( wid );
  gui::patch_viewer* viewer = new gui::patch_viewer( wid );
  viewer->resolution( resolution );


  QSlider* slider = new QSlider ( Qt::Horizontal,  wid );
  slider->setMaximum( resolution );

  QObject::connect( slider, SIGNAL(valueChanged( int ) ),
		    viewer, SLOT( value( int ) ) );
  
  QObject::connect( slider, SIGNAL(valueChanged( int ) ),
		    viewer, SLOT( updateGL( ) ) );
  
  lay->addWidget(viewer);
  lay->addWidget(slider);
  
  wid->setLayout( lay );
  wid->show();

  return app.exec();
}
