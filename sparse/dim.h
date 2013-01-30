#ifndef SPARSE_DIM_H
#define SPARSE_DIM_H

#include <sparse/types.h>
#include <math/types.h>

namespace sparse {

  template<class K>
  math::natural dim(K k) { return traits<K>::dimension(k); }
  
}

#endif
