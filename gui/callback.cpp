#include "callback.h"

namespace gui {


  callback::callback() {
    
  }

  void callback::trigger() {
    if( action ) action();
  }
  
}
