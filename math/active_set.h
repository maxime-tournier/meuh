#ifndef MATH_ACTIVE_SET_H
#define MATH_ACTIVE_SET_H

#include <math/vec.h>

namespace math {

  
  struct active_set {

    // size is @n, only the @n - @start last entries are processed
    active_set(natural n, natural start = 0);

    // active constraints
    vec mask;

    // bilateral flag
    vec bilateral;
    
    bool update(const vec& primal, // constraint error
		const vec& dual);   // lagrange multipliers


    struct step_type {
      vec point, dir;
    };
    

    // give old constraint residual Ax - b, returns step size
    static real step(const vec& old, const vec& desired);

    // minimum step to keep old + alpha * dir >= 0, alpha in [0, 1]
    real clamp(const step_type& primal, const step_type& dual) const;

    template<class Out, class In>
    void filter(Out&& out, const In& other) const {
      std::forward<Out>(out) = mask.cwiseProduct(other);
    }

    vec filter(const vec& v) const;

  };


}


#endif
