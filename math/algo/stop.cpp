#include "stop.h"


#include <iostream>


namespace math {
  namespace algo {


    void stop::debug() const {
      std::cout << "iterations : " << iterations 
		 << " precision: " << epsilon << std::endl;
    }

  }
}
