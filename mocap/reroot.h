#ifndef MOCAP_REROOT_H
#define MOCAP_REROOT_H

#include <mocap/skeleton.h>
#include <mocap/pose.h>

#include <memory>
#include <set>
#include <map>

namespace mocap {
  
  
  struct reroot {
    reroot(mocap::skeleton* skel, mocap::skeleton* j);
    ~reroot();
    
    const std::unique_ptr<mocap::skeleton> skeleton;
    
    const std::map<mocap::skeleton*, mocap::skeleton*> node_map;

    // nodes that were reversed
    const std::set<mocap::skeleton* > changed;

    typedef pose domain;
    typedef pose range;
    
    // absolute mapping
    range operator()(const domain& ) const;
    math::T<range> operator()( const math::T<domain>& ) const;

  };


}


#endif
