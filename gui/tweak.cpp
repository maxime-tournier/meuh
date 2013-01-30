#include "tweak.h"


#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

#include <gui/fix.h>

namespace gui {
  
  tweaker<double>::~tweaker() { }
  tweaker<unsigned int>::~tweaker() { }
    
    
  tweaker<double>::tweaker(double init) : init(init) { }
  tweaker<unsigned int>::tweaker(unsigned int init) : init(init) { }
  tweaker<bool>::tweaker(bool init) : init(init) { }

  QWidget* tweaker<double>::widget() {
    if(box) return box.get();
      
    box.reset( new QDoubleSpinBox );
      
    
    box->setSingleStep( init / 10.0 );

    const double scale = 10000.0;

    if( init > 0 ) {
      box->setMinimum( -init * scale );
      box->setMaximum( init * scale );
    } else {
      box->setMaximum( -init * scale );
      box->setMinimum( init * scale );
    }
    box->setValue( init );
   
    box->setDecimals(5);

    
      
    QObject::connect( box.get(), SIGNAL( valueChanged(double)),
		      &slot, SLOT( get( double ) ));
      
  
    return box.get();
  }


  QWidget* tweaker<unsigned int>::widget() {
    if(box) return box.get();
      
    box.reset( new QSpinBox );
      
  
    box->setMinimum(0);

    box->setValue(init);
    QObject::connect( box.get(), SIGNAL( valueChanged(int)),
		      &slot, SLOT( get(int) ));
    
    return box.get();
  }



   QWidget* tweaker<bool>::widget() {
     if(box) return box.get();
     
     box.reset( new QCheckBox("Enable") );
     box->setChecked(init);

     QObject::connect( box.get(), SIGNAL( stateChanged(int)),
		       &slot, SLOT( get(int) ));
    
     return box.get();
   }


  
    
}
 
