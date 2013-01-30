#ifndef MATH_KKT_H
#define MATH_KKT_H

#include <math/qp.h>
#include <math/vec.h>
#include <math/algo/pgs.h>
#include <math/lcp.h>

namespace math {

  class kkt {
    const math::qp qp;
    const mat Qinv;
  public:
    const math::lcp lcp;
    
    kkt(const math::qp& );
    kkt(math::qp&& );
    
    vec lcp_rhs(const vec& c, const vec& b) const;
    vec qp_sol(const vec& c, const vec& lambda) const;
    
    struct solver;
  };

  
  class kkt::solver {
    const math::kkt kkt;
    const algo::pgs pgs;

    mutable math::vec last;
  public:

    solver(const math::kkt& kkt);
    solver(math::kkt&& kkt);
    
    // TODO init
    vec solve(const vec& c, const vec& b,
	      const algo::stop& stop = algo::stop() ) const;
    
  };

}


#endif
