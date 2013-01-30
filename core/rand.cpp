#include "rand.h"

#include <ctime>

namespace core {

  static uint w = 10; 
  static uint z = w;
  
  uint rand() {
    z = 36969 * (z & 65535) + (z >> 16);
    w = 18000 * (w & 65535) + (w >> 16);
    return (z << 16) + w;  /* 32-bit result */
  }
  

}
