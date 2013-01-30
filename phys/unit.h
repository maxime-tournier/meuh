#ifndef PHYS_UNIT_H
#define PHYS_UNIT_H

#include <math/real.h>

namespace phys {
  
  namespace unit {

    // easy: 
    // if i mean one second: 1 * s;
    // if i want x in seconds: x / s;


    // TODO box types ?

    typedef math::real mass;
    const mass g = 1e-3;
    const mass kg = 1e3 * g;
    const mass T = 1e3 * kg;

    typedef math::real length;
    const length m = 1;
    const length dm = 1e-1 * m;
    const length cm = 1e-2 * m;
    const length mm = 1e-3 * m;
  
    typedef math::real volume;
    const volume m3 = m * m * m;
    const volume cm3 = cm * cm * cm;

    typedef math::real force;
    const force N = 1;

    typedef math::real time;
    const time s = 1;
    const time ms = 1e-3 * s;
  
    typedef math::real freq;
    const freq Hz = 1 / s;
    const freq KHz = 1e3 * Hz;
    
  }
}



#endif
