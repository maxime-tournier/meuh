#ifndef MATH_EUCLID_H
#define MATH_EUCLID_H

#include <math/types.h>

#include <core/use.h>

#include <utility>
#include <cmath>

#include <cassert>

namespace math {

  namespace euclid {
    template<class E>
    class space {
      traits<E> impl;

      void constraints() {
	E x = zero();
	E y = minus( x );
	E z = sum(x, y);
	
	field lambda = 0.0;
	z = scal(lambda, x);
	
	const E w = z;

	natural i = dim();
	
	field& x0 = coord(0, x);
	const field& w0 = coord(0, w);
	
	static_assert( std::is_same<E, typename space< dual >::dual >::value,
		       "the dual of the dual must be the primal");
	
	space< dual > d = *(*this);
	space< E > p = *d;
	
	core::use(x, y, z, lambda, d, p, x0, w0, i);
      };
    
    public:
      template<class ... Args>
      space(Args&&... args) : impl( std::forward<Args>(args)... ) { }
      space() : impl() {}	// TODO is this needed ?
      
      space( const space& ) = default;
      space( space& other ) : impl( other.impl ) { }
      space( space&& ) = default;

      space( const space&& other) : impl( std::move(other.impl) ) { }

      ~space() { core::use( &space::constraints ); }

      typedef typename traits<E>::field field;

      auto zero() const -> decltype( impl.zero() ) { 
	return impl.zero();
      }

      auto minus(const E& e) const -> decltype( impl.minus( e ) ) {
	return impl.minus( e );
      }

      auto sum(const E& x, const E& y) const -> decltype( impl.sum( x, y ) ) {
	return impl.sum( x, y );
      }
  
      auto scal(field lambda, const E& x) const -> decltype( impl.scal(lambda, x) ) {
	return impl.scal( lambda, x );
      }
    
      natural dim() const { return impl.dim(); }
      
      field& coord(natural i, E& e) const { return impl.coord(i, e); }
      const field& coord(natural i, const E& e) const { return impl.coord(i, e); }
      
      typedef typename traits<E>::dual dual;
      space< dual > operator*() const { return *impl; }

      // easy accessors
      template<class F>
      void each(const E& x, const F& f) const {
	for(natural i = 0, n = dim(); i < n; ++i) {
	  f( coord(i, x) );
	}
      };

      template<class F>
      void each(E& x, const F& f) const {
	for(natural i = 0, n = dim(); i < n; ++i) {
	  f( coord(i, x) );
	}
      };



      // coordinates extraction  TODO resize ?!
      template<class Vector>
      void get(Vector&& out, const E& e) const {
	assert(out.size() == dim());

	natural i = 0;
	each(e, [&](const field& xi) {
	    std::forward<Vector>(out)(i++) = xi;
	  });

      }
    
      template<class Vector>
      void set(E& e, const Vector& in ) const {
	assert(in.size() == dim());
	
	natural i = 0;
	each(e, [&](field& xi) {
	    xi = in(i++);
	  });
	
      }

      field dot(const E& x, const E& y) const {
	field res = 0;

	for(natural i = 0, n = dim(); i < n; ++i) {
	  res += coord(i, x) * coord(i, y);
	}

	return res;
      }
      
      field norm2(const E& x) const {
	return dot(x, x);
      }

      field norm(const E& x) const {
	return std::sqrt( norm2(x) );
      }
      

      // canonical pairing
      field pair(const dual&x, const E& y) const {
	auto d = this->operator*();
	field res = 0;

	for(natural i = 0, n = dim(); i < n; ++i) {
	  res += d.coord(i, x) * coord(i, y);
	}

	return res;
      }


      // transpose
      dual transp(const E& x) const {
	auto d = this->operator*();
	dual res = d.zero();
	
	natural i = 0;
	each(x, [&](const field& xi) {
	    d.coord(i++, res) = xi;
	  });
	
	return res;
      }
    };

    // maybe rename get ?
    template<class E>
    space<E> of(const E& e) { return {e}; }
    

    // convenience - canonical geometry
    template<class E>
    typename space<E>::field dot(const E& x, const E& y) { 
      typename space<E>::field res = 0;
      auto e = of(x);
      for(natural i = 0, n = e.dim(); i < n; ++i) {
	res += e.coord(i, x) * e.coord(i, y);
      }

      return res;
    }

    template<class E>
    typename space<E>::field norm2(const E& x) { 
      return dot(x, x);
    }
    
    template<class E>
    typename space<E>::field norm(const E& x) {
      return std::sqrt( norm2(x) );
    }


    template<class E>
    typename space<E>::dual pair(const E& x) {
      return of(x).transp(x);
    }
    
    template<class E>
    typename space<E>::field pair(const E& x, const typename space<E>::dual& y) {
      return of(y).pair(x, y);
    }

    
    // TODO L2 L1 Linf norms, angle
    // TODO zero check, equal, orthogonal, ...

    // coordinates extraction 
    template<class E, class Vector>
    void get(Vector&& out, const E& e) { of(e).get(out, e); }
    
    template<class E, class Vector>
    void set(E& e, const Vector& in ) { of(e).set(e, in); }

    
    // operators
    template<class E>
    auto minus(const E& e) -> decltype( of(e).minus(e) ){ return of(e).minus(e); }

    template<class E>
    auto operator-(const E& e) -> decltype( minus(e) ){ return minus(e); }
    
    template<class E>
    auto sum(const E& x, const E& y) -> decltype( of(x).sum(x, y) ) { return of(x).sum(x, y); }
    
    template<class E>
    auto operator+(const E& x, const E& y) -> decltype( sum(x, y) ) { return sum(x, y); }
    
    template<class E>
    auto diff(const E& x, const E& y) -> decltype( x + (-y) ) { return x + (-y); }

    template<class E>
    auto operator-(const E& x, const E& y) -> decltype( diff(x, y) ) { return diff(x, y); }
    
    
    template<class E>
    auto scal(const typename space<E>::field& lambda, const E& x) -> decltype( of(x).scal(lambda, x) ){ 
      return of(x).scal(lambda, x); 
    }

    template<class E>
    auto operator*(const typename space<E>::field& lambda, const E& x) -> decltype( scal(lambda, x) ){ 
      return scal(lambda, x); 
    }
    
    
    template<class E>
    auto operator*(const E& x, const typename space<E>::field& lambda) -> decltype( of(x).scal(lambda, x) ) { 
      return lambda * x; 
    }
    
    template<class E>
    auto operator/(const E& x, const typename space<E>::field& lambda) -> decltype( of(x).scal(1.0/lambda, x) ) { 
      return x * (1.0/ lambda); 
    }
    
    // convenience
    template<class E>
    natural dim(const E& e) { return of(e).dim(); }

  }

  
}



#endif
