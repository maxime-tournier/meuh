#ifndef MOCAP_PACK_H
#define MOCAP_PACK_H

#include <string>

#include <mocap/clip.h>
#include <mocap/curves.h>
#include <mocap/reroot.h>

namespace mocap {
  struct pack {
    pack(const std::string& filename);
    
    const std::string filename;
    const mocap::clip clip;
    const mocap::curves curves;
    const mocap::reroot reroot;

    const mocap::curves::type reroot_global;
    const mocap::curves::type reroot_local;
  };
}



#endif
