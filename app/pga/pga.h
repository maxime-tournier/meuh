#ifndef GEO_H
#define GEO_H


#include <tool/viewer.h>

#include <script/api.h>

#include <gui/frame.h>
#include <core/callback.h>

#include <math/so3.h>
#include <math/lie/pga.h>


struct viewer : tool::viewer {
 
  void init();
  void draw();
  void animate();
  
  void record( bool );
  void compute();

  script::api app;
  
  viewer();
  
  math::lie::pga<math::SO3> pga;
  math::vec coords;
  
  gui::frame* f;

  std::vector< math::SO3 > recorded;
  std::vector< math::SO3>::iterator play;

  bool recording;

  math::real time, dt;
};



#endif
