#ifndef MATRIX_H
#define MATRIX_H

#include <math/mat.h>
#include <sparse/matrix.h>
#include <core/dll.h>

class Solver;

class DLL Matrix {
  sparse::mat_chunk data;
  friend class Solver;
  Matrix(sparse::mat_chunk);

public:
  Matrix();

  Matrix& operator=(const Matrix& other);
  
  double get(unsigned int i, unsigned int j) const;
  void set(unsigned int i, unsigned int j, double value);
  
  unsigned int rows() const;
  unsigned int cols() const;
  
};

#endif
