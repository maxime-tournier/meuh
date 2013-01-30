#ifndef GUI_PUSH_H
#define GUI_PUSH_H

#include <math/types.h>
#include <QTimer>
#include <math/vec.h>

namespace gui {


  class push {
    QTimer timer;
    math::natural _frames;
  public:
    
    push(); 

    math::vec3 local;
    math::natural dof;
    
    math::vec3 dir;
    math::real stiffness;


    math::force force() const;
    bool active() const;

    void go(math::real duration);
    

    void frames(math::natural f);
    void pop();
  };

}


#endif
