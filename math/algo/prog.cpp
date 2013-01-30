#include "prog.h"

#include <math/lcp.h>

#include <math/algo/cg.h>
#include <math/algo/iter.h>

#include <iostream>

namespace math {
  namespace algo {

    // BROKEN LOL !

    // // view x, z as exponentials, then optimizes so that Mx + q -z = 0
    // // and xTz = 0 using Gauss-Newton
    // vec solve(const math::lcp& lcp, const vec& rhs, const algo::stop& stop) {
      
    //   if(rhs.rows() != int(lcp.dim()) ) throw error("caca");
      
    //    const natural n = lcp.dim();
       
    //    auto one = vec::Ones(n);
       
    //    vec v = vec::Zero(2*n), x = one, z = one;
       
    //    vec f, c = vec::Zero(2*n), lambda = vec::Zero( 2*n );
       
    //    // jacobian transpose multiplication
    //    auto Jt = [&](const vec& u)  {
    // 	 vec t(2*n);
    // 	 t << 
    // 	 x.cwiseProduct(lcp.M.transpose() * u.head(n)) + u.tail(n),
    // 	 z.cwiseProduct( - u.head(n))  + u.tail(n);
	 
    // 	 return t;
    //    };
       
    //    // jacobian multiplication
    //    auto J = [&](const vec& t) {
    // 	 vec res(2 * n);
	 
    // 	 res << 
    // 	 lcp.M * x.cwiseProduct(t.head(n)) - z.cwiseProduct(t.tail(n)),
    // 	 t.head(n) + t.tail(n);
	 
    // 	 return res;
    //    };

    //    // for least squares
    //    auto JJt = [&](const vec& u) { return J(Jt(u)); };
    //    auto JtJ = [&](const vec& u) { return Jt(J(u)); };
       

    //    algo::iter(stop, [&]() {
    // 	   f.noalias() = lcp.M * x - rhs - z;
    // 	   c << -f, -one;
	   
    // 	   // (underconstrained) least-squares
    // 	   const natural cg_iter = n / 2;
    // 	   lambda = algo::cg( JJt ).solve( c, algo::stop().it( cg_iter).eps(1e-1), lambda);
    // 	   v = Jt(lambda);
	   
    // 	   // integration
    // 	   x.array() *= (v.head(n)).array().exp();
    // 	   z.array() *= (v.tail(n)).array().exp();
	   
    // 	   // std::cout << "target: " << f.norm() << std::endl;
    // 	 });

       
    //    return x;

    // }


  }
}
