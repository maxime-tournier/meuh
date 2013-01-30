#ifndef MATH_TUPLE_H
#define MATH_TUPLE_H

#include <math/lie.h>
#include <tuple>

#include <tuple/traits.h>
#include <tuple/pow.h>

#include <cassert>

namespace math {

  
  namespace euclid {

    template<class ... Args>
    struct traits< std::tuple<Args...> > {
      typedef std::tuple<Args...> E;

      typedef typename tuple::traits<E>::range_type each;

      typedef std::tuple< space<Args>... > impl_type;
      impl_type impl;
      
      traits(const impl_type& impl = impl_type() ) : impl(impl) { }
      traits(const E& e) : impl( each::map( typename helper::space{e} ) )  { }
      traits(const space<Args>& ... impl) : impl( std::make_tuple(impl...) ) { }
      
      // choose the first as field
      typedef typename space< typename std::tuple_element< 0, E >::type >::field field;
      
      // field sanity check
      static_assert( std::is_same< std::tuple< typename space<Args>::field... >,
				   typename tuple::pow<field, sizeof...(Args) >::type
				   >::value, "arguments must be vector spaces on the same field");
   
      struct helper {
	struct zero {
	  const impl_type& impl;

	  template<int I>
	  auto operator()() const -> decltype(std::get<I>(impl).zero()) {
	    return std::get<I>(impl).zero();
	  }
	
	};

	struct space {
	  const E& e;
	
	  template<int I>
	  auto operator()() const -> decltype( of( std::get<I>(e) ) ){
	    return of( std::get<I>(e) );
	  }

	};

	struct minus {
	  const impl_type& impl;
	  const E& arg;

	  template<int I>
	  auto operator()() const -> decltype( std::get<I>(impl).minus( std::get<I>(arg) )  ) {
	    return std::get<I>(impl).minus( std::get<I>(arg) );
	  }
	};

	struct sum {
	  const impl_type& impl;
	  const E& lhs;
	  const E&rhs;
	  
	  template<int I>
	  auto operator()() const -> decltype( std::get<I>(impl).sum( std::get<I>(lhs), std::get<I>(rhs) ) ) {
	    return std::get<I>(impl).sum( std::get<I>(lhs), std::get<I>(rhs) );
	  }
	};

	struct scal {
	  const impl_type& impl;

	  const field lambda;
	  const E& arg;
	  
	  template<int I>
	  auto operator()() const -> decltype( std::get<I>(impl).scal(lambda, std::get<I>(arg) ) ) {
	    return std::get<I>(impl).scal(lambda, std::get<I>(arg) );
	  }
	};

	struct dual {
	  const impl_type& impl;

	  template<int I>
	  auto operator()() const -> decltype( *std::get<I>(impl) ) {
	    return *std::get<I>(impl);
	  }
	};

	struct dim {
	  const impl_type& impl;
	  natural& res;

	  template<int I>
	  void operator()() const {
	    res += std::get<I>(impl).dim();
	  }
	
	};

	// TODO coord ! FUU !

      };
      

      auto zero() const -> decltype( each::map( typename helper::zero{impl} ) ) {
	return each::map( typename helper::zero{impl} );
      }
      
      auto minus(const E& x) const -> decltype( each::map( typename helper::minus{impl, x} ) ) {
	return each::map( typename helper::minus{impl, x} );
      }

      auto sum(const E& x, const E& y) const -> decltype( each::map( typename helper::sum{impl, x, y} ) ) {
	return each::map( typename helper::sum{impl, x, y} );
      }

      auto scal(field lambda, const E& x) const -> decltype( each::map( typename helper::scal{impl, lambda, x} ) ) {
	return each::map( typename helper::scal{impl, lambda, x} );
      }

      typedef std::tuple< typename space<Args>::dual ... > dual;
      
      space< dual > operator*() const {
	return { each::map( typename helper::dual{impl} ) };
      }

      natural dim() const {
	natural res = 0;
	each::apply( typename helper::dim{impl, res} );
	return res;
      }

      template<class Space, class Field>
      struct get_coord {
	const impl_type& impl;
	Space& x;
	
	natural& i;
	Field*& res;
	
	template<int I>
	void operator()() const {
	  
	  const natural d = std::get<I>( impl ).dim();

	  if( d > i ) {		
	    // found
	    assert(!res);
	    
	    auto& impl_i = std::get<I>(impl);
	    auto& x_i = std::get<I>(x);

	    res = &impl_i.coord(i, x_i );
	  }
	  
	  i -= d;
	  
	}

      };

      
      // coordinates: watch out for O(n) overhead
      field& coord(natural i, E& x) const {
	typename tuple::traits<E>::range_type reverse;
	
	field* res = 0;
	reverse.apply( get_coord<E, field>{impl, x, i, res} );
	
	return *res;
      }

      const field& coord(natural i, const E& x) const {
	typename tuple::traits<E>::range_type reverse;
	
	const field* res = 0;
	reverse.apply( get_coord<const E, const field>{impl, x, i, res} );
	
	return *res;
      }



    };



  }

  namespace func {
   
    template<class ... > struct tuple;
    
  }

  
  namespace lie {

    template<class ... Args>
    struct traits< std::tuple<Args...> > {

      typedef std::tuple<Args...> G;

      typedef typename ::tuple::traits<G>::range_type each;
      
      typedef std::tuple< group<Args>... > args_type;
      args_type args;

      struct get_sub {
	const G& g;

	template<int I>
	auto operator()() const -> decltype(of(std::get<I>(g))) {
	  return of(std::get<I>(g));
	}
      };

      traits(const args_type& args = args_type()) : args(args) { }
      traits(const G& g) : args( each::map( get_sub{g} ) ) { }
      traits(const group<Args>& ... args) : args(args...) { }
      
      typedef std::tuple< typename group<Args>::algebra... > algebra;
      
      struct get_id {
	const args_type& args;

	template<int I>
	auto operator()() const -> decltype(std::get<I>(args).id()) {
	  return std::get<I>(args).id();
	}

      };

      auto id() const -> decltype( each::map( get_id{args} ))  {
	return each::map( get_id{args} );
      }

      struct get_inv {
	const args_type& args;
	const G& x;

	template<int I>
	auto operator()() const -> decltype(std::get<I>(args).inv( std::get<I>(x) ) ) {
	  return std::get<I>(args).inv( std::get<I>(x) );
	}

      };

      auto inv(const G& g) const -> decltype( each::map( get_inv{args, g} ))  {
	return each::map( get_inv{args, g} );
      }

      struct get_prod {
	const args_type& args;
	const G& lhs;
	const G& rhs;

	template<int I>
	auto operator()() const -> decltype( std::get<I>(args).prod( std::get<I>(lhs), std::get<I>(rhs) ) ) {
	  return std::get<I>(args).prod( std::get<I>(lhs), std::get<I>(rhs) );
	}
	
      };

      auto prod(const G& x, const G& y) const -> decltype( each::map( get_prod{args, x, y} ) ) {
	return each::map( get_prod{args, x, y} );
      }

      
      struct adjoint : func::base< func::tuple< typename group<Args>::adjoint... > > {

	struct get_ad {
	  const G& g;
	  
	  template<int I>
	  auto operator()() const -> decltype( ad( std::get<I>(g) ) ){
	    return ad( std::get<I>(g) );
	  }
	};

	adjoint( const G& g ) : adjoint::base( each::map( get_ad{g}) ) { }
      };

      struct coadjoint : func::base< func::tuple< typename group<Args>::coadjoint... > > {

	struct get_adT {
	  const G& g;
	  
	  template<int I>
	  auto operator()() const -> decltype( adT( std::get<I>(g) ) ){
	    return adT( std::get<I>(g) );
	  }
	};

	coadjoint( const G& g ) : coadjoint::base( each::map( get_adT{g}) ) { }
      };
      

      struct exponential : func::base< func::tuple< typename group<Args>::exponential... > > {
	
	struct get_exp {
	  const args_type& args;
	  
	  template<int I>
	  auto operator()() const -> decltype( std::get<I>(args).exp() ){
	    return std::get<I>(args).exp();
	  }
	  
	};
	
	exponential(const group<G>& g) : exponential::base( each::map( get_exp{g.impl.args} ) ) { }
      };


      struct logarithm : func::base< func::tuple< typename group<Args>::logarithm... > > {
	
	struct get_log {
	  const args_type& args;
	  
	  template<int I>
	  auto operator()() const -> decltype( std::get<I>(args).log() ){
	    return std::get<I>(args).log();
	  }
	  
	};
	
	logarithm(const group<G>& g) : logarithm::base( each::map( get_log{g.impl.args} ) ) { }
      };
      

      struct get_alg {
	const args_type& args;

	template<int I>
	auto operator()() const -> decltype(std::get<I>(args).alg()) {
	  return std::get<I>(args).alg();
	}
      };
      
      euclid::space< algebra > alg() const {
	return each::map( get_alg{args} );
      }
      
    };

  }


  namespace impl {

    template<class ... Args>
    struct tuple_nan {
      const std::tuple<Args...>& args;
      bool& res;

      template<int I>
      void operator()() const {
	res = res || nan( std::get<I>(args) );
      }
      
    };

  }
  
  template<class ... Args>
  bool nan(const std::tuple<Args...>& args) {
    bool res = false;
    tuple::each(args).apply( impl::tuple_nan<Args...>{args, res} );
    return res;
  }




}


#endif
