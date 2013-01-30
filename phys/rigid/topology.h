#ifndef PHYS_RIGID_TOPOLOGY_H
#define PHYS_RIGID_TOPOLOGY_H

#include <phys/rigid/body.h>
#include <phys/rigid/joint.h>

#include <core/graph.h>

namespace phys {
  namespace rigid {
    namespace topology {

      typedef core::graph< boost::undirectedS, body, joint >  undirected;
      typedef core::graph< boost::bidirectionalS, body, joint > directed;
      
      // gives a tree ordering to an undirected articulated rigid body
      // topology
      directed direct(undirected g, undirected::vertex_descriptor root = 0);
      
    }
  }
}
    



#endif
