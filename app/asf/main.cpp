
#include <gui/app.h>
#include "viewer.h"
 
int main(int argc, char** argv) {
  gui::app app(argc, argv);
  
  if(argc == 1) return 0;
  std::string filename = argv[1];
  
  viewer wid;
  wid.show();
  
  return app.exec();
} 


