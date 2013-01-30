#ifndef GUI_CLIP_VIEWER_H
#define GUI_CLIP_VIEWER_H


#include <gl/gl.h>

#include <gui/frame.h>
#include <gui/alpha_viewer.h>
#include <vector>

#include <memory>

#include <math/lie/pga.h>

#include <tool/mocap.h>
#include <tool/anim.h>

namespace gui {

  class clip_viewer : public alpha_viewer {
    Q_OBJECT;
    
    std::unique_ptr< tool::mocap > mocap;
    
    std::unique_ptr<qglviewer::Camera> light;
    // math::lie::pga<phys::skel::pose> pga;
  
    
    public:
    clip_viewer(QWidget* parent = 0);
    
    void init();
    void draw();
	       
    math::natural ngeo;

    tool::anim::control anim;
					     
  public slots:
    void load( const QString& );
  };

}

#endif
