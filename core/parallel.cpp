#include "parallel.h"

#include <cassert>
#include <core/log.h>



namespace core {

  static unsigned int _concurrency = 4;
  
  unsigned int concurrency() { return _concurrency; }
  void concurrency(unsigned int i) { assert(i); _concurrency = i; }
  
}
