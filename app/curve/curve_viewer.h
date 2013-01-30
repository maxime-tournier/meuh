#ifndef CURVE_VIEWER_H
#define CURVE_VIEWER_H

#include <math/func/interp.h>

#include <gl/gl.h>
#include <gui/frame.h>
#include <gui/alpha_viewer.h>
#include <vector>

#include <memory>


namespace gui {

  class curve_viewer : public alpha_viewer {
    std::vector< frame* > nodes_;

    Q_OBJECT
    
  public:

    curve_viewer(QWidget* parent = 0);

    void init();
    void animate();

    void draw();


    

  protected:
    
    typedef std::map< math::real, math::SE3, 
		      std::less<math::real>, Eigen::aligned_allocator<math::SE3> > nodes_type;
    nodes_type nodes() const;
									  
									  
  public slots:
    void add_key();
    
    
    

  };

}




#endif
