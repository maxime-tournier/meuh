#ifndef MATH_RANDOM_H
#define MATH_RANDOM_H

#include <math/types.h>
#include <functional>

namespace math {


  template<class T>
  struct random;


  template<>
  struct random< real > {
    
    typedef std::function<real() > generator;
    
    static generator uniform(real start = 0, real end = 1);

  };


  template<>
  struct random< natural > {
      
    typedef std::function< natural () > generator;
    static generator uniform(natural low = 0, natural high = 1);
    
  };
  

  
  // DOES NOT WORK FOR DYNAMIC ARRAYS !
  template<class E, int M>
  struct random< vector<E, M> > {

    typedef std::function<vector<E, M> ()> generator;
    
    static generator uniform(double start =0 , double end = 1) {
     	
      typename random<E>::generator gen = random<E>::uniform(start, end);
      
      return [gen]() -> vector<E, M> { 
	vector<E, M> res;
	res.each([&](natural i) {
	    res(i) = gen();
	  });
	
	return res;
      };
    }
      
  };


  // convenience
  template<class U>
  U rand(U low, U up) { return random<U>::uniform(low, up)(); }

}



#endif
