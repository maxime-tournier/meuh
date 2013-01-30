#include <gl/gl.h>

#include "viewer/control.h"
#include <gui/frame.h>

#include <QApplication>
#include <QSlider>

#include <QObject>
#include <QLabel>
#include <QCheckBox>
#include <QDockWidget>
#include <QMainWindow>
#include <QVBoxLayout>

#include <boost/program_options.hpp>

#include <script/console.h>
#include <script/command.h>

#include <core/raii.h>
#include <core/share.h>

int main(int argc, char** argv) {
  
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("subject,s", po::value<unsigned int>(), "mocap subject")
    ("trial,t", po::value<unsigned int>(), "mocap trial")
    ("exec,e", po::value<std::string>(), "script to execute")
    ;
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm); 

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }


  QApplication app( argc, argv );
  
  QMainWindow win;
  gui::viewer::control* viewer = new gui::viewer::control(&win);
  win.setCentralWidget( viewer );
  
  // if( argc == 3 )  {
  //   viewer->ngeo = std::stoi(argv[2]);
  // }
  win.setWindowTitle("PGA Control");

  script::console console(viewer->script());
  auto start = core::raii::start(console);
  
  if( vm.count("exec") ) {
    viewer->script().exec(script::cmd("script = ", script::str(vm["exec"].as<std::string>())));
  }
  
  viewer->init_script();
  
  if( vm.count("subject") && vm.count("trial") ) {
    viewer->script().exec( script::cmd("bvh.load(", core::string( vm["subject"].as<unsigned int>() ),
				       ", ", core::string( vm["trial"].as<unsigned int>() ),
				       ")") );
  }
  
  win.addDockWidget(Qt::LeftDockWidgetArea, viewer->animation.widget());
  win.addDockWidget(Qt::RightDockWidgetArea, viewer->controller.widget());
  win.addDockWidget(Qt::LeftDockWidgetArea, viewer->params.widget());

  win.show();

  
  
  return app.exec();
}
