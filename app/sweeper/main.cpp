#include <QApplication>
#include <QVBoxLayout>


#include "sweeper.h"

int main(int argc, char** argv) {
  QApplication app( argc, argv );

  gui::sweeper* viewer = new gui::sweeper;
  viewer->show();

  return app.exec();
}

