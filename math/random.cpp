#include "random.h"

#include <random>
#include <math/vec.h>

#include <chrono>
#include <core/seed.h>

namespace math {
  
  random<real>::generator random<real>::uniform(real start, real end) {
    
    std::uniform_real_distribution<real> distribution(start, end);
    std::default_random_engine engine( core::seed() );
    return std::bind(distribution, engine);
  }

  

  random<natural>::generator random<natural>::uniform(natural start, natural end) {
    
    std::uniform_int_distribution<int> distribution(start, end);
    std::default_random_engine engine( core::seed() );
    return std::bind(distribution, engine);
  }



  
}


