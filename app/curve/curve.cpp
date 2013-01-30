
#include "curve_viewer.h"

#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include <QObject>
#include <QPushButton>


int main(int argc, char** argv) {
  QApplication app( argc, argv );
  
  QWidget* wid = new QWidget;
  QVBoxLayout* lay = new QVBoxLayout;

  int resolution = 1000;
    

// gui::screw_viewer* viewer = new gui::screw_viewer( wid );
  // gui::patch_viewer* viewer = new gui::patch_viewer( wid );
  gui::curve_viewer* viewer = new gui::curve_viewer( wid );
  viewer->resolution( resolution );

  QString filename(argv[1]);
  
  QSlider* slider = new QSlider ( Qt::Horizontal,  wid );
  slider->setMaximum( resolution );

  QPushButton* button = new QPushButton("&Add", wid);
  

  QObject::connect( slider, SIGNAL(valueChanged( int ) ),
		    viewer, SLOT( value( int ) ) );
  
  QObject::connect( button, SIGNAL(clicked() ),
		    viewer, SLOT( add_key() ) );
  

  QObject::connect( slider, SIGNAL(valueChanged( int ) ),
		    viewer, SLOT( updateGL( ) ) );
  
  lay->addWidget(viewer);
  lay->addWidget(slider);
  lay->addWidget(button);
  
  wid->setLayout( lay );
  wid->show();

  return app.exec();
}
