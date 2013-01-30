#ifndef GUI_SWEEPER_H
#define GUI_SWEEPER_H

#include <gl/begin.h>
#include <math/vec.h>
#include <math/so3.h>

#include <tool/simulator.h>
#include <tool/sweeper.h>

#include <gui/frame.h>
#include <phys/object.h>

namespace gui {


  struct sweeper  : tool::simulator {
    
    
    void init();
    void draw();
    
    // manipulation frames
    std::vector<gui::frame*> frames;
    
    // (local) mesh vertices
    math::vector< math::vec3 > points;
   
    // (global) mesh vertices
    math::vector<math::vec3> pos, vel, norm;
    
    typedef phys::object< tool::sweeper::dof_type > obj_type;
    std::unique_ptr< obj_type > obj;
    
    void reset();

    void setup();
    phys::dof::momentum momentum(math::real t, math::real dt);

    phys::dof::velocity solve(const phys::dof::momentum& );
    void integrate(const phys::dof::velocity& , math::real dt);

    phys::system system;
    phys::engine engine;
  };
  
  

  

}


#endif
