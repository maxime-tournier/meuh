#include "kkt.h"

#include <math/svd.h>

namespace math {

  kkt::kkt(const math::qp& qp) 
    : qp(qp),
      Qinv( svd(qp.Q).inverse() ),
      lcp(qp.A * Qinv.transpose() * qp.A.transpose()) 
  { 
    
  }

  kkt::kkt(math::qp&& qp) 
    : qp(std::move(qp)),
      Qinv( svd(qp.Q).inverse() ),
      lcp(qp.A * Qinv.transpose() * qp.A.transpose()) 
  { 
    
  }

  vec kkt::qp_sol(const vec& c, const vec& lambda) const {
    assert(c.rows() == qp.Q.cols());
    assert(lambda.rows() == qp.A.rows());
    
    return Qinv.transpose() * (qp.A.transpose() * lambda - c);
  }

  vec kkt::lcp_rhs(const vec& c, const vec& b) const {
    assert(b.rows() == qp.A.rows());
    assert(c.rows() == qp.Q.cols());

    return b + qp.A * (Qinv.transpose() * c);
  }



  kkt::solver::solver(const math::kkt& kkt)
    : kkt(kkt),
      pgs(kkt.lcp) {

  }

  kkt::solver::solver(math::kkt&& kkt)
    : kkt(std::move(kkt)),
      pgs(kkt.lcp) {
    
  }


  vec kkt::solver::solve(const vec& c, const vec& b,
			 const algo::stop& stop) const {
    const vec lambda = pgs.solve( kkt.lcp_rhs(c, b), stop, last );
    assert( !nan(lambda) );

    const vec res = kkt.qp_sol( c, lambda );  
    assert(! nan(res) );
    
    last = std::move(lambda);

    return res;
  }




}
