#ifndef VECTOR_H
#define VECTOR_H


#include <sparse/vector.h>
#include <core/dll.h>

class Solver;

class DLL Vector {
  sparse::vec_chunk data;
  Vector(sparse::vec_chunk);

  friend class Solver;
public:
  Vector();
  Vector& operator=(const Vector& );
  
  double get(unsigned int i) const;
  void set(unsigned int i, double value);
  unsigned int dim() const;
};

#endif
