#ifndef MATH_FUNC_LAMBDA_H
#define MATH_FUNC_LAMBDA_H

#include <core/use.h>
#include <type_traits>

namespace math {
  namespace func {

    template<class Lambda>
    typename std::result_of<Lambda()>::type lambda(const Lambda& f) {
	
      // this prevents context-dependent lambdas
      typename std::result_of<Lambda()>::type (*check)() = f; core::use(check);

      return f();
    };

  }
}




#endif
