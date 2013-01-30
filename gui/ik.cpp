
#include <gl/gl.h>

#include <gui/ik_viewer.h>
#include <gui/frame.h>

#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include <QObject>
#include <QLabel>
#include <QCheckBox>

int main(int argc, char** argv) {
  QApplication app( argc, argv );
  
  if( argc < 2 ) return -1;

  QWidget* wid = new QWidget;
  QVBoxLayout* lay = new QVBoxLayout;

  gui::ik_viewer* viewer = new gui::ik_viewer( wid );
  
  if( argc == 3 )  {
    viewer->ngeo = std::stoi(argv[2]);
  }

  
  QString filename(argv[1]);
  viewer->load(filename);
     

  lay->addWidget(viewer);
  
  struct {
    QWidget* wid;
    QHBoxLayout* lay;
    QCheckBox* check;
    QLabel* lab;
  } fog;

  fog.wid = new QWidget;
  fog.lay = new QHBoxLayout;
  fog.check = new QCheckBox;
  fog.lab = new QLabel("fog");
  fog.lay->addWidget(fog.check);
  fog.lay->addWidget(fog.lab);
  fog.wid->setLayout(fog.lay);
  
  // lay->addWidget( fog.wid );

  wid->setLayout( lay );
  
  wid->show();

  return app.exec();
}
