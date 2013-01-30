#ifndef MATH_PSD_H
#define MATH_PSD_H

#include <math/types.h>

namespace math {
  
  template<class A>
  struct psd;


  template<>
  struct psd< mat > {
    const mat& M;

    psd(const mat& M);

    vec operator()(const vec& v) const;
    natural dim() const;

  };

}


#endif
