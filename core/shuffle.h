#ifndef CORE_SHUFFLE_H
#define CORE_SHUFFLE_H

#include <vector>
#include <core/uint.h>

#include <chrono>
#include <functional>

#include <core/seed.h>

namespace core {


  template<class Value>
  void shuffle(std::vector<Value>& what) {

    for(uint i = 0, n = what.size(); i < n; ++i) {
      std::uniform_int_distribution<uint> dist(i, n-1);
      std::default_random_engine engine( seed() );
     
      std::swap( what[i], what[ dist(engine) ] );
    }

  }



}


#endif
