#ifndef VIEWER_H
#define VIEWER_H

#include <tool/simulator.h>
#include "cloth.h"
#include <memory>

#include <gui/frame.h>

class viewer : public tool::simulator {
  
  typedef ::cloth cloth_type;
  typedef cloth_type::obj_type obj_type;
  
  std::unique_ptr< cloth_type > cloth;

  void bilateral();

  struct pinned_type {
    phys::constraint::bilateral::key key;
    // phys::dof::key dof;
    math::natural index;
    math::real rest;
  };
    
  std::map<gui::frame*, pinned_type > pinned;
  std::vector<gui::frame*> frame;
  

  phys::constraint::bilateral::vector lambda;
  
  math::natural post_steps;

  script::api api;

  math::natural iterations;

public:
  
  viewer();
  
  void setup( std::unique_ptr< ::cloth >&& c);
  
  void init();
  void draw();
  void reset();

  void compliance(math::real c);
};



#endif
