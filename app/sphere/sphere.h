#ifndef SPHERE_H
#define SPHERE_H

#include <tool/viewer.h>
#include <core/callback.h>

struct viewer : tool::viewer {
  

  void init();
  void draw();

  core::callback on_draw;
};



#endif
