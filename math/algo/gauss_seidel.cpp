#include "gauss_seidel.h"

namespace math {
  namespace algo {
  
    gs_solve::gs_solve(const mat& m) : gauss_seidel(m) { }

    vec gs_solve::operator()(const vec& b, const vec& init, const stopper& stop) const {
      return gauss_seidel::operator()(b, init, projector::none(), stop);
    }

    vec gs_solve::operator()(const vec& b, const stopper& stop ) const {
      return operator()(b, vec::Zero(b.rows()), stop);
    }


    gs_lcp_solve::gs_lcp_solve(const mat& m) : gauss_seidel(m) { }

    vec gs_lcp_solve::operator()(const vec& b, const vec& init, const stopper& stop) const {
      return gauss_seidel::operator()(b, init, projector::positive(), stop);
    }

    vec gs_lcp_solve::operator()(const vec& b, const stopper& stop) const {
      return operator()(b, vec::Zero(b.rows()), stop);
    }

  }  
}
