#ifndef PHYS_STATE_H
#define PHYS_STATE_H

#include <math/lie.h>

#include <math/func/tuple.h>
#include <math/func/any.h>
#include <math/func/partial.h>
#include <math/func/compose.h>
#include <math/func/vectorized.h>

#include <math/func/gradient.h>
#include <math/func/numerical.h>

#include <math/mat.h>

namespace phys {
 
  template<class G>
  struct state {

    typedef typename math::lie::group<G>::algebra algebra;
    typedef typename math::lie::group<G>::coalgebra coalgebra;

    typedef std::tuple< G, algebra > type;
    
    typedef math::func::tuple_get<0, type> config_type;
    typedef math::func::tuple_get<1, type> velocity_type;
    
    static config_type conf() { return {}; }
    static velocity_type vel() { return {}; }
    
    typedef math::func::any<type, math::real> lagrangian_type;
    
    // dL / dv(at)

    typedef math::func::any<algebra, coalgebra> mass_type;
    static mass_type mass(const lagrangian_type& L, const type& at) {
      using namespace math::func;
      return grad(L << part<1>(at));
    }
    
    
    static math::mat matrix(const mass_type& m, const math::lie::group<G>& lie = math::lie::group<G>() ) {
      auto alg = lie.alg();
      auto coalg = lie.coalg();

      using namespace math;      
      auto M = func::vectorize(m, alg, coalg);
      
      mat res;
      natural n = alg.dim();

      res.resize(n, n);
      
      auto task = [&](natural start, natural end) {
	vec delta = vec::Zero(n);
	auto copy = M;

	for(natural i = start; i < end; ++i) {
	  delta(i) = 1;
	  res.col(i) = copy(delta);
	  delta(i) = 0;
	}

      };

      // task(0, n);
      core::parallel(0, n, task);

      return res;
    }

    typedef std::function< math::mat (const G& ) > mass_matrix_type;
    static  mass_matrix_type mass_matrix(const lagrangian_type& L, const type& at) {
      auto lie = math::lie::of(at);
      auto alg = lie.alg();
      auto coalg = lie.coalg();
      
      return [L, alg, coalg](const G& at) -> math::mat {
	using namespace math;
	auto M = func::vectorize( state::mass(L, at), alg, coalg );
	
	natural n = alg.dim();
	vec delta = vec::Zero( n );
	
	math::mat res;
	res.resize(n, n);

	res.each_col([&](natural j) {
	    delta(j) = 1;
	    res.col(j) = M(delta);
	    delta(j) = 0;
	  });	
	
	return res;
      };
      
    };

    // dL / dq(at)
    typedef math::func::any<G, coalgebra> force_type;
    static force_type force(const lagrangian_type& L, const type& at) {
      using namespace math::func;

      // return grad( L << part<0>(at) ); 
      return grad( num( L << part<0>(at), 1e-1 ));
    }
    

    typedef std::tuple< G, algebra, coalgebra > bundle_type;

    typedef std::function< bundle_type ( const bundle_type& , math::real dt) > integrator_type;
  };

}


#endif
