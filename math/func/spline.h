
#ifndef MATH_FUNC_SPLINE_H
#define MATH_FUNC_SPLINE_H

#include <math/func/hermite.h>
#include <math/func/lie.h>
#include <math/tuple.h>

#include <math/error.h>
// #include <core/tuple/foreach.h>
#include <math/func/lie/subgroup.h>
#include <math/func/alias.h>

namespace math {
  namespace func {
    namespace spline {
      
      // TODO directly take tuples to cut some crap ?
      namespace coeffs {

	// as seen on http://en.wikipedia.org/wiki/Cubic_Hermite_spline
	// p(t) = s0(t) * p0 + s1(t) * m0 + s2(t) * p1 + s3(t) * m1
	// coefficients for points(pi) and tangents(mi)
	template<class U>
	class hermite {
	public:
	  typedef U domain;
	  typedef typename ::tuple::pow<domain, 4>::type range;
	  
	private:
	  
	  struct scaling {
	    const U start;
	    const U end;
	    const U width;
	    
	    scaling( U start, U end) 
	      : start(start), 
		end(end),
		width(end - start)
	    { 
	      if( start >= end ) throw math::error("bounds must be strictly increasing");
	    }
	    
	    typedef U domain;
	    typedef U range;

	    range operator()( const domain& x ) const {
	      assert( x >= start );
	      assert( x <= end );
	      return ((id<U>() - val(start)) / width )(x);
	    }

	    struct push : base< func::euclid::scalar<domain> > {
	      push(const scaling& of, const domain& ) 
		: push::base( 1.0 / of.width ) {

	      }
	    };


	    typedef push pull;	    
	  };

	  const scaling scale;
	 
	  
	public:
	  hermite(const U& start = 0, const U& end = 1) 
	    : scale(start, end)
	  {
	    
	  }
	  
	  
	  range operator()(const domain& u) const {
	    return ( func::join(  func::hermite::h00, 
				  scale.width * func::hermite::h10, 
				  func::hermite::h01, 
				  scale.width * func::hermite::h11) 
		     << scale ) (u);
	  }
	  
	  // T<range> diff(const T<domain>& du) const {
	  //   return ( func::join(  h00(), scale.width * h10(), h01(), scale.width * h11())
	  // 	     << scale ).diff (du);
	  // }
	  
	};


	// cubic spline with tension
	// coefficients for points
	// !!! equally spaced !!! (see wikipedia, cubic spline, TODO: finite difference)
	template<class U>
	class cardinal {
	  
	  // rearrange hermite terms to get cardinal terms
	  struct from_hermite {
	    typedef typename hermite<U>::range domain;
	    typedef domain range;
	    
	    const U alpha;	// this is (1 - tension) / 2*width
	    
	    range operator()(const domain& c) const {
	      range res;
	      
	      // c is: p0, m0, p1, m1
	      // res is: p-1, p0, p1, p2
	      using std::get;
	      get<0>(res) = -alpha * get<1>(c);
	      get<1>(res) = ( get<0>(c) - alpha * get<3>(c) );
	      get<2>(res) = ( get<2>(c) + alpha * get<1>(c) );
	      get<3>(res) = alpha * get<3>(c);
	      
	      return res;
	    }
	    
	    struct push : base< from_hermite > {
	      push(const from_hermite& of, const domain& at) : from_hermite(of) { };
	    };

	  };

	  typedef compose< from_hermite, coeffs::hermite<U> > impl_type;
	  const impl_type impl;
	  
	public:
	  typedef U domain;
	  typedef typename coeffs::hermite<U>::range range;
	  
	  cardinal(U tension = 0, U start = 0, U end = 1) 
	    : impl( from_hermite{ (1.0 - tension) /  (2*(end - start)) } << coeffs::hermite<U>( start, end ) )   { } 
	  
	  range operator()(const domain& u) const {
	    return impl( u );
	  }
	  
	  
	  // TODO !
	  struct push : base< typename traits<impl_type>::push > {
	    
	    push(const cardinal& of, const domain& at) 
	      :  push::base( d(of.impl)(at) ) { }
	    
	  };

	  struct pull : base< typename traits<impl_type>::pull > {
	    
	    pull(const cardinal& of, const domain& at) 
	      :  pull::base( dT(of.impl)(at) ) { }
	    
	  };
	  
	};
	
	
	template<class U = real>
	struct catmull_rom : cardinal<U> { 
	  
	  catmull_rom(U start = 0, U end = 1) : cardinal<U>(0, start, end) { }
	  
	  
	};

	

	// cumulative form, see http://portal.acm.org/citation.cfm?id=218380.218486
	template<class U = real>
	struct cumulative {
	  typedef typename coeffs::hermite<U>::range domain;
	  typedef domain range;
	  
	  range operator()(const domain& x) const {
	    range res;
	    using namespace std;
	    using std::get;
	    
	    get<3>(res) = get<3>(x);
	    get<2>(res) = get<3>(res) + get<2>(x);
	    get<1>(res) = get<2>(res) + get<1>(x);
	    get<0>(res) = get<1>(res) + get<0>(x);
	    
	    return res;
	  }

	
	  struct push : base< cumulative > {
	    push(const cumulative& of, const domain& at) : cumulative(of) {  }
	  };

	  struct pull : base< cumulative > {
	    pull(const cumulative& of, const domain& at) : cumulative(of) {  }
	  };
	  
	};
	

	template<class Coeffs>
	compose< cumulative<typename Coeffs::domain >, Coeffs> cumulate( const Coeffs& coeffs ) {
	  return cumulative< typename Coeffs::domain >() << coeffs;
	}
	
      }


      // a general spline patch in Lie groups, see
      // http://portal.acm.org/citation.cfm?id=218380.218486
      template<class G, class Coeffs = coeffs::catmull_rom<> >
      struct patch {

	typedef typename Coeffs::domain domain;
	typedef G range;
	
	typedef typename math::lie::group<G>::algebra algebra;
	
	static_assert( std::is_same< domain, typename math::euclid::space<algebra>::field >::value,
		       "ROFLMAO !");
	

	
	typedef lie::subgroup<G> sub;
	typedef func::tuple<sub, sub, sub, sub> subgroups_base;
	
	struct subgroups : subgroups_base {
	  
	  static typename ::tuple::pow<G, 4>::type 
	  diffs( const typename ::tuple::pow<G, 4>::type& val,
		 const math::lie::group<G>& lie) {
	    using namespace std;
	    
	    return std::make_tuple( std::get<0>(val), 
				    lie.prod( lie.inv( std::get<0>(val) ), std::get<1>(val) ),
				    lie.prod( lie.inv( std::get<1>(val) ), std::get<2>(val) ),
				    lie.prod( lie.inv( std::get<2>(val) ), std::get<3>(val) ));
	  }
	  
	  struct get_sub {
	      
	    const typename ::tuple::pow<G, 4>::type& diffs;
	    const math::lie::group<G>& lie;

	    template<int I>
	    sub operator()() const {
	      return { lie.log()( std::get<I>(diffs) ), lie };
	    }

	  };

	  static typename ::tuple::pow<sub, 4>::type 
	  subs( const typename ::tuple::pow<G, 4>::type& diffs,
		const math::lie::group<G>& lie) {
	    return subgroups_base::each::map( get_sub{diffs, lie} );
	  }
	  
	  subgroups(const std::tuple<G, G, G, G>& nodes, const math::lie::group<G>& lie) 
	    : subgroups_base( subs( diffs(nodes, lie), lie) ) {
	    
	  }
	  
	};
	

	const subgroups exps;
	const Coeffs coeffs;
	const lie::product<G, 4> prod;
	
	typedef std::tuple<G, G, G, G> nodes_type;

	patch(const nodes_type& nodes, 
	      const Coeffs& coeffs = Coeffs(), 
	      const math::lie::group<G>& lie = math::lie::group<G>()) 
	  : exps( nodes, lie ),
	    coeffs( coeffs ),
	    prod( lie )
	{
	  
	}

			       
	range operator()(const domain& t) const {
	  return (prod << exps << spline::coeffs::cumulate(coeffs) )(t);
	}
	

	// TODO
	// T<range> diff(const T<domain>& dt) const {
	//   return (prod << exps << spline::coeffs::cumulate(coeffs)).diff (dt); 
	// }


      };


      // helper
      template<class G, class Coeffs>
      patch<G, Coeffs> make_patch(const std::tuple<G, G, G, G>& points,
				  const Coeffs& coeffs, const math::lie::group<G>& lie = math::lie::group<G>()) {
	return patch<G, Coeffs>( points, coeffs, lie );
      } 
      
    }
  }
}


#endif
