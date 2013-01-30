#ifndef TRAJ_H
#define TRAJ_H

#include <tool/viewer.h>

#include <tool/trajectory.h>
#include <script/bundle.h>

#include <gui/frame.h>
#include <core/callback.h>

#include <mutex>
#include <set>

struct viewer : tool::viewer {

  void init();
  void draw();
  
  tool::trajectory curve;
  script::bundle app;

  viewer();


  void add_editor(math::real t, math::natural level);

  std::mutex mutex;
  std::set< gui::frame* > frames;
  std::list< core::callback > on_draw;
};



#endif
