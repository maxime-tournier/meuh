#ifndef PATCH_VIEWER_H
#define PATCH_VIEWER_H

#include <gl/gl.h>
#include <gui/alpha_viewer.h>
#include <vector>

namespace gui {

  struct frame;

  class patch_viewer : public alpha_viewer {
    std::vector<frame*> nodes_;
    
  public:
    patch_viewer(QWidget* parent = 0);

    void init();
    void draw();

  };

}

#endif
