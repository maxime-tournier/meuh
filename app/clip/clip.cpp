
#include <gl/gl.h>

#include "clip_viewer.h"
#include <gui/frame.h>

#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include <QObject>
#include <gui/playbar.h>

#include <phys/unit.h>

int main(int argc, char** argv) {
  QApplication app( argc, argv );
  
  if( argc < 2 ) return -1;


  QWidget* wid = new QWidget;
  QVBoxLayout* lay = new QVBoxLayout;

  int resolution = 5000;

  gui::clip_viewer* viewer = new gui::clip_viewer( wid );
  viewer->resolution( resolution );
  viewer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  if( argc == 3)  viewer->ngeo = std::stoi(argv[2]);
    
  QString filename(argv[1]);
  viewer->load(filename);

  auto playbar = new gui::playbar(&viewer->anim);
  viewer->anim.interval( 1 / (100 * phys::unit::Hz) );
  
  // QSlider* slider = new QSlider ( Qt::Horizontal,  wid );
  // slider->setMaximum( resolution );
  
  // QObject::connect( slider, SIGNAL(valueChanged( int ) ),
  // 		    viewer, SLOT( value( int ) ) );
  
  // QObject::connect( slider, SIGNAL(valueChanged( int ) ),
  // 		    viewer, SLOT( updateGL( ) ) );
  
  lay->addWidget(viewer);
  lay->addWidget(playbar);
  
  wid->setLayout( lay );
  wid->show();

  return app.exec();
}
