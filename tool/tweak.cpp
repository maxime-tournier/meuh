#include "tweak.h"

#include <QDoubleSpinBox>
#include <gui/slot.h>
#include <script/meuh.h>

#include <core/log.h>

namespace tool {
  
  tweak::tweak() 
    : api("tweak") {

    api
      .fun("real", [&](const std::string& name,
		       math::real lower,
		       math::real upper) {
	     this->real(name, lower, upper);
	   })
      ;
  }

  
  struct chunk_base {
    virtual ~chunk_base() { }
    
    QWidget* widget;
  };
  

  template<class T>
  struct chunk : chunk_base {
    gui::slot<T> slot;
  };
  
  static std::list< std::unique_ptr<chunk_base> > chunks;
  
  void tweak::real(const std::string& lua_name,
		   math::real lower,
		   math::real upper) {
    assert( upper >= lower );
    
    chunk<math::real>* c = new chunk<math::real>;
    
    QDoubleSpinBox* box = new QDoubleSpinBox;
    
    box->setSingleStep( (upper - lower) / 10.0 );
    box->setMinimum(lower);
    box->setMaximum(upper);
    
    QObject::connect( box, SIGNAL( valueChanged(double)),
		      &c->slot, SLOT( get( double ) ));
    
    c->slot.action = [lua_name]( math::real x ) {
      // core::log("tweaked lol", x); 
      script::exec(lua_name + " = %%", x);
    };
    
    c->widget = box;
    chunks.push_back( std::unique_ptr<chunk_base>(c) );

    box->show();
  }

}
