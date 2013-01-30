#ifndef MATH_PULLBACK_H
#define MATH_PULLBACK_H

#include <math/cotangent.h>

namespace math {
  
  // pullback bundle
  template<class F>
  class pullT {
    typedef typename F::domain base;
    typedef typename Lie<typename F::range>::coalgebra coalgebra;
    
    const F& f;
    const base at_;
    const coalgebra body_;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    const F& ref() const { return f; }
    
    pullT(const F& f, const base& at, const coalgebra& body) 
      : f(f), at_(at), body_(body) { }

    pullT(const F& f, const base& at, coalgebra&& body) 
      : f(f), at_(at), body_(std::move(body)) { }

    pullT(const F& f, base&& at, coalgebra&& body) 
      : f(f), at_(std::move(at)), body_(std::move(body)) { }
    
    const base& at() const { return at_; }
    const coalgebra& body() const { return body_; }
    coalgebra spatial() const {  math::spatial( f( at() ), body() ).spatial(); }
    
  };
  
  // convenience constructors
  template<class F>
  pullT<F> body(const F& f, const typename F::domain& at, 
	       const typename Lie<typename F::range>::coalgebra& body) {
    return pullT<F>(f, at, body);
  }

  template<class F>
  pullT<F> body(const F& f, const typename F::domain& at, 
	       typename Lie<typename F::range>::coalgebra&& body) {
    return pullT<F>(f, at, std::move(body));
  }
  
  template<class F>
  pullT<F> body(const F& f, typename F::domain&& at, 
	       typename Lie<typename F::range>::coalgebra&& body) {
    return pullT<F>(f, std::move(at), std::move(body));
  }
  
  template<class F>
  pullT<F> spatial(const F& f, const typename F::domain& at, 
		  const typename Lie<typename F::range>::coalgebra& spatial) {
    return pullT<F>(f, at, math::spatial( f(at), spatial).body() );
  }
  
  

}


#endif
