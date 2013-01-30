#ifndef MOCAP_SKELETON_H
#define MOCAP_SKELETON_H

#include <core/tree.h>
#include <mocap/joint.h>

namespace mocap {

  typedef core::tree::node< joint > skeleton;
  
  // gives joints an id, returns joint count
  joint::index number(skeleton* skel);
  
  skeleton* find( skeleton* skel, const std::string& name);

  
};

#endif
