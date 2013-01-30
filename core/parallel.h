#ifndef CORE_PARALLEL_H
#define CORE_PARALLEL_H

#include <cassert>
#include <algorithm>

namespace core {
  
 
  unsigned int concurrency();
  void concurrency(unsigned int);
  
  // split action in n chunks, compute it in parallel
  template<class Action>
  void parallel(int start, int end, const Action& action, unsigned int n = concurrency() ) {
    assert(n > 0);
    assert(end > start);
    
    const unsigned int length = end - start;
    n = std::min(n, length);
    
#pragma omp parallel for
    for(unsigned int i = 0; i < n; ++i) {
      const int s = start + ( length * i) / n;
      const int e = start + ( length * (i + 1)) / n;
      
      action(s, e);
    }
    
  }
      

  
}



#endif
