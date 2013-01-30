#include "menu.h"

#include <QMenu>
#include <core/stl.h>

#include <gui/slot.h>

#include <gui/fix.h>

namespace gui {

  void menu::popup(const QPoint& where) const {

    QMenu* menu = new QMenu;
    // menu->setTitle( title.c_str() ); // this does nothing :-/

    assert( actions.find( title ) == actions.end() && "clash in menu entries");    
    const std::string full = title + "          "; // hack to get hruler
    QAction* dummy = menu->addAction(full.c_str());
    dummy->setEnabled( false );
    
    QFont font = dummy->font();
    font.setBold( true );
    font.setUnderline( true );
    dummy->setFont( font );

    // TODO garbage collect
    core::each(actions, [&](const std::string& name, const std::function< void() >& callback) {

	if( !core::is_hook(name) ) {
	  QAction* act = menu->addAction(name.c_str());
	  auto trig = new gui::slot<>(menu);
	  trig->action = callback;
	  
	  QObject::connect(act, SIGNAL(triggered()),
			   trig, SLOT(get()));
	}
	
      });
    
    menu->popup(where);
  }
  
}
