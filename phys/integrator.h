#ifndef PHYS_INTEGRATOR_H
#define PHYS_INTEGRATOR_H

#include <phys/state.h>
#include <math/iter.h>
#include <math/minres.h>

#include <math/func/vectorized.h>

namespace phys {

  namespace integrator {
    
    namespace euler {

      template<class G>
      typename state<G>::integrator_type symplectic(const typename state<G>::lagrangian_type& L,
						    const math::lie::group<G>& lie = math::lie::group<G>(),
						    math::natural steps = 1) {
	
	typedef phys::state<G> state_type;
	typedef typename state_type::bundle_type bundle_type;

	const auto alg = lie.alg();
	const auto coalg = lie.coalg();

	const math::natural n = alg.dim();
	
	typedef typename state_type::algebra algebra;
	typedef typename state_type::coalgebra coalgebra;

	auto res = [L, n, lie, alg, coalg, steps](const bundle_type& x_k, math::real dt) {
	  using namespace math::func;

	  tuple_get<0, bundle_type> g;
	  tuple_get<1, bundle_type> v;
	  tuple_get<2, bundle_type> p;

	  const auto exp = lie.exp();
	  const auto log = lie.log();

	  // current state
	  const auto state = join(g, v)(x_k);
	  
	  // mass tensor at g_k
	  const auto mass = state_type::mass(L, state);
	 	  
	  using namespace math;
	  
	  auto M = func::vectorize(mass, alg, coalg);
	  
	  const G d_k = exp( alg.scal(dt, v(x_k)) );
	  const coalgebra pulled_pk = lie.adT(d_k) ( dT( log )( lie.inv(d_k) )( p(x_k) ) );
	  
	  // current velocity estimate
	  algebra v_tmp = v(x_k);

	  // dense vectors for solving
	  math::vec rhs = math::vec::Zero(n);
	  math::vec sol = math::vec::Zero(n);
	  
	  for(natural i = 0; i < steps; ++i) {

	    // potential/inertial force at g_k with current velocity estimate
	    const auto f_k = state_type::force(L, std::make_tuple(g(x_k), v_tmp))( g(x_k) );
	    
	    // rhs
	    coalgebra net = coalg.sum(pulled_pk, coalg.scal(dt, f_k) );
	   
	    // rhs pullback 
	    const algebra v_next = v_tmp;
	    const algebra minus_e_next = alg.scal(-dt, v_next);
	    
	    // pulled by current estimate
	    net = dT( exp )( minus_e_next ) ( net );
	    
	    // fill dense 
	    coalg.get(rhs, net);
	    alg.get(sol, v_tmp);
	    
	    // solver setup
	    math::iter iter;
	    
	    iter.bound = 10;
	    iter.epsilon = 1e-3;
	    
	    sol = math::minres(M).solve(rhs, iter, sol );
	    alg.set(v_tmp, sol);
	  }

	  bundle_type x_next;
	  
	  auto& g_next = std::get<0>(x_next);
	  auto& v_next = std::get<1>(x_next);
	  auto& p_next = std::get<2>(x_next);
	  
	  v_next = v_tmp;
	  g_next = lie.prod(g(x_k), exp(alg.scal(dt, v_next)) );
	  p_next = mass( v_next );
	  
	  return x_next;
	};
	
	return res;
      }
      
    }
  }
}


#endif
