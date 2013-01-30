#include "vector.h"

Vector::Vector() : data(0, 0) { 

}

Vector::Vector(sparse::vec_chunk data) 
  : data(data) { 
 
}

double Vector::get(unsigned int i) const {
  assert( data.data() );
  return data(i);
}

void Vector::set(unsigned int i, double d) {
  assert( data.data() );
  data(i) = d;
}

unsigned int Vector::dim() const { return data.rows(); }


Vector& Vector::operator=(const Vector& other) {
  new (this) Vector(other.data);
  return *this;
}
