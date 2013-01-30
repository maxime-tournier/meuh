#ifndef LAGRANGIAN_H
#define LAGRANGIAN_H

#include <tuple>
#include <math/real.h>

namespace phys {

  namespace lagrange {
    
    template<class L>
    struct map;

    template<class L>
    typename map<L>::momentum_type momentum(const L& );
    
    template<class L>
    typename map<L>::potential_type potential(const L& );
    
    
    // template<class G>
    // struct base : math::function::base< std::tuple<G, typename math::lie::map<G>::algebra >, 
    // 					math::space::real > {
      
    // };
  
    
    // template<class G>
    // math::function::identity< std::tuple<G, typename math::lie::map<G>::algebra > > start() {
    //   return math::function::identity< std::tuple<G, typename math::lie::map<G>::algebra > >();
    // }
    
  };
}

#endif
