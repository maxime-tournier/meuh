#ifndef SCREW_VIEWER_H
#define SCREW_VIEWER_H


#include <gui/frame.h>
#include <gui/alpha_viewer.h>

namespace gui {

  class screw_viewer : public alpha_viewer {
    frame* target_;
    frame* interp_;
  public:
    screw_viewer(QWidget* parent = 0);

    void init();
    void draw();
  };

}

#endif
