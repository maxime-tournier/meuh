#include "type_map.h"

#include <core/stl.h>

namespace core {

  type_map::base::~base() { }

  void type_map::clear() {

    core::each( core::all(impl), [&](key_type , base* b) {
	delete b;
      });
    
    impl.clear();
  }

  type_map::~type_map() {
    clear();
  }

}
