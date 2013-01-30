#ifndef MATH_FUNCTION_LIE_H
#define MATH_FUNCTION_LIE_H

#include <math/lie.h>
// #include <math/tangent.h>

#include <tuple/pow.h>

#include <math/func/tuple.h>
#include <math/func/compose.h>
#include <math/func/coord.h>

#include <math/euclid.h>

#include <core/log.h>

// TODO break in smaller parts, add Ad 
namespace math {
  namespace func {
    
    namespace lie {

      template<class G, int N = 2>
      struct product {
	
	typedef typename ::tuple::pow<G, N>::type domain;
	typedef G range;
	
	math::lie::group<G> lie;
	typedef typename ::tuple::traits<domain>::range_type each;
	
	product(const math::lie::group<G>& lie = math::lie::group<G>() ) 
	  : lie(lie) { }

	struct apply {
	  range& res;
	  const math::lie::group<G>& lie;
	  const domain& x;
	  
	  template< int I >
	  void operator()() const {
	    res = lie.prod(res, std::get<I>(x));
	  }
	  
	};


	range operator()(const domain& x) const {
	  range res = lie.id();
	  
	  each::apply( apply{res, lie, x} );

	  return res;
	}


	struct get_at {
	    
	  mutable G acc;
	  
	  const math::lie::group<G>& lie;
	  const domain& at;

	  template<int I>
	  G operator()() const {

	    auto res = lie.inv(acc);
	    acc = lie.prod(std::get<I>(at), acc);
	    
	    return res;
	  }
	    
	};
	

	struct push {

	  typedef typename math::lie::group<G>::algebra algebra;
	  
	  const math::euclid::space<algebra> alg;

	  typedef typename math::lie::group<domain>::adjoint ad_type;
	  const ad_type ad;
	

	  push(const product& of, const domain& at) 
	    : alg( of.lie.alg() ),
	      ad( each::map( get_at{of.lie.id(), of.lie, at} ) ) {

	  }

	  struct apply {
	    algebra& res;

	    const math::euclid::space<algebra>& alg;
	    const ad_type& ad;
	    
	    const typename math::lie::group<domain>::algebra& v;
	    
	    template<int I>
	    void operator()() const {
	      res = alg.sum(res, std::get<I>(ad.args)( std::get<I>(v) ) );
	    }
	    

	  };

	  typename math::lie::group<range>::algebra operator()(const typename math::lie::group<domain>::algebra& v) const {
	    
	    algebra res = alg.zero();
	    each::apply( apply{res, alg, ad, v});
	    return res;
	  }

	};


	struct pull {
	  
	  typedef typename math::lie::group<domain>::coadjoint adT_type;
	  const adT_type adT;
	  
	  
	  pull( const product& of, const domain& at) 
	    : adT( each::map(get_at{of.lie.id(), of.lie, at}) )
	  {
	    
	  }


	  struct get_result {
	    const typename math::lie::group<range>::coalgebra& f;

	    const adT_type& adT;

	    template<int I>
	    typename math::lie::group<G>::coalgebra operator()() const {
	      auto& adT_i =  std::get<I>(adT.args);
	      return adT_i( f );
	    }
	    
	  };
	  
	  typename math::lie::group<domain>::coalgebra operator()(const typename math::lie::group<range>::coalgebra& g) const {
	    return each::map( get_result{g, adT} );
	  }
	  
	  
	};
	
	
      };

      template<class A, class ... Args>
      compose< product<  typename traits< typename std::decay<A>::type >::range, 1 + sizeof...(Args) >,
	       tuple_join< typename std::decay<A>::type, typename std::decay<Args>::type... > >
      prod(A&& a, Args&& ... args) {
	return product< typename traits< typename std::decay<A>::type >::range, 1 + sizeof...(Args) >{} 
	<< join(std::forward<A>(a), std::forward<Args>(args)...);
      }

      // // convenience
      // template<class A, class ... Args>
      // compose< product< typename traits<A>::range, 1 + sizeof...(Args) >,
      // 	       tuple_join<A, Args...> >
      // prod(const A& a, const Args&... args) {
      // 	return product< typename A::range, 1 + sizeof...(Args) >() 
      // 	  << func::tuple_join<A, Args...> (a, args ... );
      // }      
      
      
      
      template<class G>
      struct inverse {
	
	typedef G domain;
	typedef G range;
	
	math::lie::group<G> lie;
	
	inverse(const math::lie::group<G>& lie = math::lie::group<G>() ) 
	  : lie(lie) { }

	range operator()(const domain& g) const {
	  return lie.inv(g);
	}

	struct push {
	  
	  const math::euclid::space< typename math::lie::group<G>::algebra > alg;
	  const typename math::lie::group<G>::adjoint ad;
	  
	  push(const inverse& inv, const domain& at) 
	    : alg( inv.lie.alg() ),
	      ad( inv.lie.ad(at) ) {
	    
	  }

	  typename math::lie::group<G>::algebra operator()(const typename math::lie::group<G>::algebra& v) const {
	    return alg.minus( ad(v) );
	  }
	  
	};

	struct pull {
	  
	  const math::euclid::space< typename math::lie::group<G>::coalgebra > coalg;
	  const typename math::lie::group<G>::coadjoint adT;
	  
	  pull(const inverse& inv, const domain& at) 
	    : coalg( inv.lie.coalg() ),
	      adT( inv.lie.adT(at) ) {
	    
	  }

	  typename math::lie::group<G>::coalgebra operator()(const typename math::lie::group<G>::coalgebra& f) const {
	    return coalg.minus( adT(f) );
	  }
	  
	};


      };
      

      template<class G>
      struct product_vector {
      
        typedef math::vector<G> domain;
        typedef G range;

	natural n;
	math::lie::group<G> lie;

	
	product_vector(natural n, 
		       const math::lie::group<G>& lie = math::lie::group<G>())
	  : n(n),
	    lie(lie) {

	}

        range operator()(const domain& g) const {
	  assert(g.size() == n);
	  
	  G res = lie.id();
	  g.each([&](natural i) {
	      res = lie.prod(res, g(i));
	    });
	  
	  return res;
        }
	
	
	domain help(const domain& at) const {
	  
	  domain res;
	  res.resize( n );

	  G current = lie.id();
	  
	  res.each([&](natural i) {

	      res(i) = lie.inv( current );
	      current = lie.prod( at(i), current );
	      
	    });

	  return res;

	};

	struct push {
	  
	  product_vector< typename math::lie::group<range>::algebra > sum;
	  typename math::lie::group<domain>::adjoint ad;

	  push(const product_vector& of,
	       const domain& at) 
	    : sum( of.n, math::lie::of( of.lie.alg().zero() ) ), // TODO FIXME SLOW!
	      ad( of.help(at) )
	  {
	    
	  }


	  typename math::lie::group<range>::algebra operator()(const typename math::lie::group<domain>::algebra& v) const {
	    return (sum << ad)(v);
	  }
	  
	};

	struct pull {

	  pull( const product_vector& of,
	       const domain& at) 
	    : adT( of.help(at) ) {
	  }
	  
	  typename math::lie::group<domain>::coadjoint adT;

	  typename math::lie::group<domain>::coalgebra operator()(const typename math::lie::group<range>::coalgebra& f) const {
	    return math::lie::group<domain>::coalgebra::map([&](natural i ) {
		return adT.data(i)(f);
	      });
	  }
	};
	
      };

      
      //   T<range> diff(const T<domain>& dvec) const {
      // 	const natural n = dvec.at().rows(); assert(n);
      // 	assert(dvec.at().rows() == dvec.body().rows());
	
      // 	const Lie<range> lie(dvec.at()(0));
	
      // 	// we compute at from right to left 
      // 	G at = lie.id();
      // 	typename Lie<range>::algebra body = lie.alg().zero();
	
      // 	dvec.at().each([&, n](natural i) {  
      // 	    const natural k = n - 1 - i;
	    
      // 	    using math::euclid::operator+;
      // 	    body = lie.alg().sum( body, lie.ad( lie.inv(at) )( dvec.body()(k) ) );
      // 	    at = lie.prod(dvec.at()(k), at);
      // 	  });
	
      // 	return math::body(std::move(at), std::move(body));
      //   }

      //   // TODO optimize
      //   coT<domain> diffT(const pullT<product_vector>& fvec) const {
	
      // 	const natural n = fvec.at().rows(); assert( n );

      // 	const Lie<range> lie(fvec.at()(0));
	  
      // 	typename Lie<domain>::coalgebra body = math::lie::coalg(fvec.at()).zero();
	  
      // 	G tail = math::lie::of(fvec.at()(0)).id();
	
      // 	body.each([&, n](natural i) {
      // 	    const natural k = n - 1 - i;
	    
      // 	    body(k) = math::lie::adT( math::lie::inv(tail) ) (fvec.body());
      // 	    tail = lie.prod(fvec.at()(k), std::move(tail));
      // 	  });
	
      // 	return math::cobody(fvec.at(), std::move(body));
      //   }

      // };
      
    
    }
  }
}



#endif
