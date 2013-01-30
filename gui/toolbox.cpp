#include "toolbox.h"

#include <QToolBox>
#include <QWidget>
#include <QPushButton>
#include <QDockWidget>

#include <gui/slot.h>

#include <gui/fix.h>

namespace gui {

  toolbox::toolbox(const std::string& name, QWidget* parent) {
      
    toolbox_ = new QToolBox(parent);
    toolbox_->setMinimumWidth(150);
      
    dock_ = new QDockWidget(name.c_str());
    dock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock_->setWidget( toolbox_ );

    connect(this, SIGNAL(shown(bool)),
	    dock_, SLOT( setVisible(bool)) );
    
  }

  void toolbox::show( bool value ) { 
    emit shown(value);
  }
  
  
  QDockWidget* toolbox::widget() const { return dock_; }
  
  toolbox& toolbox::add(const std::string& name, QWidget* wid) {
    toolbox_->addItem(wid, name.c_str());
      
    // TODO delete callback
    return *this;
  }


  toolbox& toolbox::add(const std::string& name, const std::function< void () >& callback) {
      
    QPushButton* butt = new QPushButton("&Go");
      
    gui::slot<>* trig = new gui::slot<>;
    trig->action = callback;
      
    QObject::connect(butt, SIGNAL(clicked()),
		     trig, SLOT( get() ) );
      
    toolbox_->addItem(butt, name.c_str());
    
    return *this;
  }
    
  


}
