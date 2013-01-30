#ifndef MATH_TANGENT_H
#define MATH_TANGENT_H

#include <math/lie.h>
#include <math/error.h>

#include <math/eigen.h>

namespace math {
  
  // implementation, concept map
  template<class G>
  class T {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef typename Lie<G>::algebra algebra;

    const G& at() const { return at_; }
    
    const algebra& body() const { return body_; }
    algebra spatial() const { return Lie<G>( at() ).ad( at() ) ( body() ); }
    
    T(const G& a, const algebra& b) : at_(a), body_(b) { }
    T(G&& a, algebra&& b) : at_( std::move(a)), body_(std::move(b)) { }
    T(T&& other) : at_(std::move(other.at_)), body_(std::move(other.body_)) { }
    T(const T& other) : at_(other.at_), body_(other.body_) { }
    
  private:
    
    // these should be const, but gcc 4.5 fails to move-construct
    // const tuples from tuples rvalue ref
    G at_;
    algebra body_;
    
  };
    
  // convenience constructors
  template<class G>
  T<G> body(const G& at,
	    const typename Lie<G>::algebra& body) { 
    return {at, body};
  }

  template<class G>
  T< typename core::strip<G>::type  > 
  body(G&& at, typename Lie< typename core::strip<G>::type >::algebra&& body) { 
    return { std::forward<G>(at), std::move(body) };
  }
  
  
  template<class G>
  T<G> spatial(const G& at, const typename Lie<G>::algebra& spatial) {
    const Lie<G> lie( at );
    return body( at, lie.ad( lie.inv( at ) )( spatial) );
  }


  
  
}

#endif
