#ifndef GRAPH_H
#define GRAPH_H

#include <core/dll.h>

class DLL Graph {
 
public:
  Graph();

  enum Type{
    DFS = 1,
    MST,
    PROP,
    RND,
    PGS,
    CG,
    NONE,
    CMK,
  };

  void setType( Type );
  Type getType() const;

private:
  Type type;
};

#endif

