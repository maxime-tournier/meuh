#ifndef SPARSE_TYPES_H
#define SPARSE_TYPES_H

namespace sparse {

  template<class Key> class vector;
  template<class Row, class Col = Row> class matrix;
  template<class Key> class weight;

  template<class Key> struct traits;
  template<class Cont> struct chunk;
};


#endif
