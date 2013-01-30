#include "graph.h"

#include <core/log.h>

Graph::Graph() : type(DFS) { }

Graph::Type Graph::getType() const {
  return type;
}

void Graph::setType(Graph::Type t) { 
  type = t; 
}

