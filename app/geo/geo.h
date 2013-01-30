#ifndef GEO_H
#define GEO_H

#include <tool/viewer.h>
#include <gui/frame.h>

struct viewer : tool::viewer {
  
  viewer();

  void init();
  void draw();
  void animate();
  
  gui::frame target, frame;

  math::real t, dt;
  script::api app;
};



#endif


