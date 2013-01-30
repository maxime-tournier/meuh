#ifndef MATH_COTANGENT_H
#define MATH_COTANGENT_H

#include <math/lie.h>

namespace math {

  // implementation, concept map
  template<class G>
  class coT {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef typename Lie<G>::coalgebra coalgebra;

    const G& at() const { return at_; }
    
    const coalgebra& body() const { return body_; }
    coalgebra spatial() const { return lie::of( at() ).adT( lie::inv( at()) ) ( body() ); }
    
    coT(const G& a, const coalgebra& b) : at_(a), body_(b) { }
    coT(const G& a, coalgebra&& b) : at_(a), body_(std::move(b)) { }
    coT(G&& a, coalgebra&& b) : at_(std::move(a)), body_(std::move(b)) { }
    
  private:
    const G at_;
    const coalgebra body_;
  };
    
  
  template<class G>
  coT<G> cobody(const G& at,
		const typename Lie<G>::coalgebra& body) { 
    return coT<G>(at, body);
  }

  template<class G>
  coT<G> cobody(const G& at,
		typename Lie<G>::coalgebra&& body) { 
    return coT<G>(at, std::move(body));
  }

  template<class G>
  coT<G> cobody(G&& at,
		typename Lie< typename core::strip<G>::type >::coalgebra&& body) { 
    return coT<G>(std::forward<G>(at), std::move(body));
  }


  template<class G>
  coT<G> cospatial(const G& at, const typename Lie<G>::coalgebra& spatial) {
    const Lie<G> lie( at );
    return body( at, lie.adT( at )( spatial) );
  }

}

#endif
