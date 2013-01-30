#ifndef MATH_LIE_H
#define MATH_LIE_H

#include <math/euclid.h>

namespace math {

  namespace lie {
  
    template<class G>
    class group {
      friend struct traits<G>;
      traits<G> impl;

      void constraints() {
	G x = id();
	G y = inv( x );
	G z = prod(x, y);

	core::use(x, y, z);
      }
    
    public:
    
      group( const group<G>&) = default;
      group( group<G>& other) : impl( other.impl) { }
      
      group( group&& ) = default;
      
      
      // need this one otherwise the forward ctor might kick in
      group( const group&& other ) : impl( std::move(other.impl) ){ }; 
      
      template<class ... Args> 
      group( Args&& ... args)  : impl( std::forward<Args>(args)... ) { }
      group() : impl() { }	// TODO is this needed ?
      
    

      ~group() { core::use( &group::constraints ); }
      
      typedef typename traits<G>::algebra algebra;
      typedef typename euclid::space< algebra >::dual coalgebra;
      
      typedef typename traits<G>::adjoint adjoint;
      typedef typename traits<G>::coadjoint coadjoint;

      typedef typename traits<G>::exponential exponential;
      typedef typename traits<G>::logarithm logarithm;

      auto id() const -> decltype( impl.id() ) {
	return impl.id();
      }
    
      auto inv(const G& g) const -> decltype( impl.inv( g ) ) {
	return impl.inv(g);
      }
    
      auto prod(const G& a, const G& b) const -> decltype( impl.prod( a, b ) ) {
	return impl.prod(a, b);
      }

      euclid::space<algebra> alg() const { return impl.alg(); }
      euclid::space<coalgebra> coalg() const { return *alg(); }
    
      adjoint ad(const G& g) const { return {g}; }
      coadjoint adT(const G& g) const { return {g}; }
    
      exponential exp() const { return { *this }; }
      logarithm log() const { return { *this }; }
      
    };

    template<class G>
    group<G> of(const G& g) { return {g}; }
    
    // some convenient one-liners
    template<class G>
    auto inv( const G& g) -> decltype( of(g).inv(g) ) {
      return of(g).inv(g);
    }

    template<class G>
    auto prod( const G& a, const G& b) -> decltype( of(a).prod(a, b) ) {
      return of(a).prod(a, b);
    }

    template<class G>
    auto ad( const G& g ) -> decltype( of(g).ad( g ) ) {
      return of(g).ad( g );
    }

    template<class G>
    auto adT( const G& g ) -> decltype( of(g).adT( g ) ) {
      return of(g).adT( g );
    }
    
  }

}
  



#endif
