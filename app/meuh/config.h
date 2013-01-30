#ifndef CONFIG_H
#define CONFIG_H

#include "cg.h"
#include "graph.h"

#include <core/dll.h>

class DLL Config {
  CG _cg;
  Graph _graph;
public:

  CG& cg();
  Graph& graph();
};



#endif
