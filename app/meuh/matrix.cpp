#include "matrix.h"

Matrix::Matrix(sparse::mat_chunk data) : data(data) { }
Matrix::Matrix() : data(0, 0, 0) { }

double Matrix::get(unsigned int i, unsigned int j) const {
  assert( i < rows() );
  assert( j < cols() );

  return data(i, j);
}

void Matrix::set(unsigned int i, unsigned int j, double d) {
  assert( i < rows() );
  assert( j < cols() );

  // core::log("Matrix set", i, j, d);

  data(i, j) = d;
}

unsigned int Matrix::rows() const { return data.rows(); }
unsigned int Matrix::cols() const { return data.cols(); }


Matrix& Matrix::operator=(const Matrix& other) {
  new (this) Matrix(other.data); // WTF ?
  return *this;
}
