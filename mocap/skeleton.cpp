#include "skeleton.h"

namespace mocap {


  joint::index number(skeleton* skel ) {
    joint::index i = 0;
    
    core::tree::prefix( [&i](skeleton* j) {
	j->set().id = i++;
      }) (skel);
    
    return i;
  }


  skeleton* find(skeleton* in, const std::string& name) {
    return core::tree::find( [&name](mocap::skeleton* j ) { return j->get().name == name; }   ) ( in );
  }


  
}
