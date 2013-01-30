#ifndef GUI_MENU_H
#define GUI_MENU_H

#include <core/actions.h>

struct QPoint;

namespace gui {

  struct menu {
    std::string title;
    core::actions actions;
    void popup(const QPoint& point) const;
  };


}


#endif
