#ifndef MATH_FUNC_TUPLE_H
#define MATH_FUNC_TUPLE_H

#include <math/func.h>
#include <math/lie.h>


#include <tuple/range.h>
#include <tuple/pow.h>

#include <math/func/push.h>
#include <math/func/pull.h>
#include <math/func/ref.h>

#include <tuple/traits.h>

#include <core/local.h>

namespace math {

  namespace func {
    
    template<class ... Args>
    struct tuple {
      
      typedef std::tuple<Args...> args_type;
      args_type args;

      typedef typename ::tuple::traits<args_type>::range_type each;
      
      tuple(const Args& ... args) : args(args...) { }
      tuple(const args_type& args = args_type() ) : args(args) { }
      
      typedef std::tuple< typename traits<Args>::domain... > domain;
      typedef std::tuple< typename traits<Args>::range... > range;
      
      struct apply {
	range& res;
	const args_type& args;
	const domain& x;
	 
	template<int I>
	void operator()() const {
	  std::get<I>(res) = std::get<I>(args)( std::get<I>(x) );
	}
	
      };

      mutable range res;
      
      const range& operator()(const domain& x) const {
	each::apply( apply{res, args, x} );
	return res;
      }

     
      

      struct push : base< tuple< typename traits<Args>::push... > > {
	
	struct get_push {
	  
	  const args_type& args;
	  const domain& at;

	  template<int I>
	  auto operator()() const -> decltype( d(std::get<I>(args))( std::get<I>(at) ) ) {
	    return d( ref(std::get<I>(args)) )( std::get<I>(at) );
	  }
	};

	push(const tuple& of, const domain& at) 
	  : push::base( each::map( get_push{of.args, at} ) ) {

	}
	
      };

      struct pull : base< tuple< typename traits<Args>::pull... > > {
	
	struct get_pull {
	  
	  const args_type& args;
	  const domain& at;

	  template<int I>
	  auto operator()() const -> decltype(dT(std::get<I>(args))( std::get<I>(at) ) ) {
	    return dT( ref(std::get<I>(args)) )( std::get<I>(at) );
	  }
	};

	pull(const tuple& of, const domain& at) 
	  : pull::base( each::map( get_pull{of.args, at} ) ) {

	}
	
      };

    };

    template<class ... A>
    func::tuple<A...> make_tuple(const A&... a) { return {a...}; }


    // access i-th element in a tuple
    template<int , class> struct tuple_get;
    
    template<int I, class ... Args>
    struct tuple_get< I, std::tuple<Args...> > {

      typedef std::tuple<Args...> domain;
      typedef typename std::tuple_element<I, std::tuple<Args...> >::type range;
      
      range operator()(const domain& x) const { return std::get<I>(x); }
       
      struct push : base< tuple_get<I, typename math::lie::group<domain>::algebra > > {

	push(const tuple_get&, const domain& ) {};
	
      };

      // TODO 
      struct pull {
	mutable typename math::lie::group<domain>::coalgebra res;
	
	pull(const tuple_get&, const domain& at)
	  : res( math::lie::of(at).coalg().zero() ){
	}

	const typename math::lie::group<domain>::coalgebra& operator()(const typename math::lie::group<range>::coalgebra& f) const {
	  std::get<I>(res) = f;
	  return res;
	};
	
      };
      
    };
  
    template<int I, class ... Args>
    tuple_get<I, std::tuple<Args...> > get(const std::tuple<Args...>& ) { return {}; }
    


     // joins multiple functions defined on the same domain in a tuple
    template<class ... Args>
    struct tuple_join {
      
      static_assert( sizeof...(Args) >= 1, "no arguments provided" );

      typedef std::tuple<Args...> terms_type;
      terms_type terms;

      typedef typename ::tuple::traits< terms_type >::range_type each;
      
      tuple_join(const Args&... args) : terms( args... ) { }
      tuple_join(const terms_type& terms = terms_type() ) : terms(terms) { }
      
      typedef typename std::tuple_element<0, terms_type>::type head;
      typedef typename traits<head>::domain domain;

      typedef std::tuple< typename traits<Args>::range... > range;
      
      static_assert( std::is_same< typename ::tuple::pow< domain, sizeof...(Args) >::type, 
				   std::tuple< typename traits<Args>::domain... > >::value, 
	"function domains must all agree");
      
        
      struct apply {
	range& res;
	const terms_type& terms;
	const domain& x;

	template<int I>
	void operator()() const {
	  std::get<I>(res) = std::get<I>(terms)(x);
	}
      };
    
      mutable range res;

      const range& operator()(const domain& x) const {
	// auto& res = *this->res;
	each::apply( apply{res, terms, x} );
	return res;
      }

    struct push : base< tuple_join< typename traits< Args >::push... > > {
	
	struct get_push {
	  const terms_type& terms;
	  const domain& at;

	  template<int I>
	  auto operator()() const -> decltype( d( ref(std::get<I>(terms) ))(at) ){
	    return d( ref(std::get<I>(terms)) )(at);
	  }
	};

	push( const tuple_join& of, const domain& at)
	  : push::base( each::map( get_push{of.terms, at} ) ) { 
	  
	}
	
      };
      

      struct pull {
	
	std::tuple< typename traits<Args>::pull... > args;
	euclid::space< typename math::lie::group<domain>::coalgebra > coalg;
	
	struct sum {
	  const std::tuple< typename traits<Args>::pull... >& args;
	  const math::euclid::space< typename math::lie::group<domain>::coalgebra >& coalg;

	  const typename math::lie::group<range>::coalgebra& f;

	  typename math::lie::group<domain>::coalgebra& res;
	  
	  template<int I>
	  void operator()() const {
	    auto& f_i = std::get<I>(f);
	    auto& arg_i = std::get<I>(args);

	    const auto& pulled = arg_i(f_i);
	    
	    // FIXME real => field
	    natural i = 0;
	    coalg.each(res, [&](real& r) {
		r += coalg.coord(i++, pulled);
	      });
	    
	  }
	  
	};

	struct get_args {
	  const terms_type& terms;
	  const domain& at;
	  
	  template<int I>
	  auto operator()() const -> decltype( dT( ref(std::get<I>(terms) ))(at) ) {
	    return dT(ref( std::get<I>(terms) ) )(at);
	  }
	};


	// TODO rvalue

	mutable typename math::lie::group<domain>::coalgebra  res;
	const typename math::lie::group<domain>::coalgebra& operator()(const typename math::lie::group<range>::coalgebra& fx) const {
	  res = coalg.zero();

	  each::apply( sum{args, coalg, fx, res} );
	  return res;
	}

	pull(const tuple_join& of, const domain& at) 
	  : args( each::map( get_args{of.terms, at} ) ),
	    coalg( math::lie::of(at).coalg() ) {
	}
	

      };
      
    };

    template<class ... Args>
    tuple_join<Args...> join(const Args&... args) {
      return { args... };
    }






    
    
    // // tuple/vector conversions
    // template<class U, int I>
    // struct to_vector {
    //   typedef typename core::tuple::repeat<U, I>::type domain;
    //   typedef math::vector<U, I> range;

    //   range operator()(const domain& x) const {
    // 	range res;
    // 	unsigned int i = 0;
	
    // 	core::tuple::apply( filler(res, i), x );
    // 	return res;
    //   }


    //   struct push {
	
    // 	push(const to_vector& , const domain& at) {}
	
    // 	typename Lie<range>::algebra operator()(const typename Lie<domain>::algebra& body) const {
    // 	  typename Lie<range>::algebra res;

    // 	  unsigned int i = 0;
    // 	  core::tuple::apply( filler(res, i), body );
	  
    // 	  return res;
    // 	}

    //   };
      
    // private:
      
    //   template<class A>
    //   struct fill {
    // 	math::vector<A, I>& res;
    // 	unsigned int& i;

    // 	fill(math::vector<A, I>& r, unsigned int& ii) : res(r), i(ii) { };

    // 	template<class ... Args>
    // 	void operator()(const A& a, const Args&... args) const {
    // 	  res(i) = a;
    // 	  ++i;
    // 	  operator()(args...);
    // 	}

    // 	void operator()() const {
    // 	  assert( i==I );
    // 	}
	
    //   };

    //   template<class A>
    //   static fill<A> filler(math::vector<A, I>& res, unsigned int& i) { return fill<A>( res, i ); }
      
    // };
    

   
    
  }  
}
  
#endif
