#ifndef GUI_IK_VIEWER_H
#define GUI_IK_VIEWER_H


#include <viewer/fancy.h>

#include <phys/skel/pack.h>

#include <QGLViewer/qglviewer.h>
#include <gui/frame.h>

#include <vector>
#include <memory>

#include <mocap/ik/handle.h>

#include <gl/texture.h>
#include <math/lie/pga.h>
#include <phys/skel/pga.h>
#include <QTime>


#include <control/feet.h>

namespace gui {

  class ik_viewer : public viewer::fancy  {
        
    std::vector<gui::frame* > frames;
    // std::vector<mocap::ik::handle> handles;

    math::lie::pga<phys::skel::pose> pga;
    
    QTime time;
    
    Q_OBJECT

    phys::skel::pose pose;
    math::Lie<phys::skel::pose>::algebra body;
    
    phys::skel::pga::domain param;
    math::Lie<phys::skel::pga::domain>::algebra bparam;
    
    std::unique_ptr<phys::skel::pack> pack;
    
    math::vec3 selected;
    math::vec3 target;
    math::real dist;
    bool ik;
    

    bool fix;
    void update_target(int x, int y);

  public:
    math::natural ngeo;
    
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    public:
    ik_viewer(QWidget* parent = 0);
    
    void init();

    void draw();
    void drawWithNames();
    void postSelection(const QPoint& point);
    
    // void add_handle(mocap::skeleton*);
    void animate();
	
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyReleaseEvent(QKeyEvent* e);
    void keyPressEvent(QKeyEvent* e);
  public slots:
    void reset();
    void load( const QString& );
    
  };

}

#endif
