#ifndef MATH_FUNC_LIE_CCFK_H
#define MATH_FUNC_LIE_CCFK_H

#include <math/func/lie.h>

namespace math { 
  namespace func {
    namespace lie {


      template<class G>
      struct ccfk {

	typedef typename math::lie::group<G>::algebra algebra;
	typedef typename math::euclid::space< algebra >::field field;

	typedef math::vector<field> domain;
	typedef G range;
	
	const math::vector<algebra>& dirs;
	
	math::lie::group<G> lie;
	natural n;
	
	typename math::lie::group<G>::exponential exp;
	
	mutable typename math::lie::group<G>::algebra res;

	ccfk(const math::vector<algebra>& dirs,
	     natural nn = 0,
	     const math::lie::group<G>& lie = math::lie::group<G>()) 
	  : dirs(dirs), 
	    lie(lie),
	    n( nn ? nn : dirs.rows()  ),
	    exp( lie.exp() ),
	    res( lie.alg().zero() )
	    
	{ 
	  assert(n && (int(n) <= dirs.rows()) );
	}
	
	

	const typename math::lie::group<G>::algebra& help(const domain& x) const {
	  assert(x.rows() == int(n));
	  
	  auto alg = lie.alg();
	  
	  x.each([&](natural i) {
	      using math::euclid::operator+;
	      using math::euclid::operator*;
	      
	      natural j = 0;
	      alg.each(res, [&](real& r) {
		  auto delta = x(i) * alg.coord(j++, this->dirs(i));

		  if(i == 0) r = delta;
		  else r += delta;
		});
	      
	    });
	  
	  return res;
	}

	auto operator()(const domain& x) const 
	  -> decltype( exp( typename math::lie::group<G>::algebra{}  ) ) {
	  return exp( help(x) );
	}



	typedef typename math::lie::group<G>::exponential expontential;

	struct push  {
	  
	  typedef typename traits< expontential >::push dexp_type;

	  math::euclid::space< algebra > alg;
	  dexp_type dexp;
	  const math::vector<algebra>& dirs;
	  
	  mutable typename math::lie::group<range>::algebra res;

	  push(const ccfk& of,
	       const domain& at)
	    : alg(of.lie.alg()),
	      dexp( d(of.exp)(of.help(at))),
	      dirs(of.dirs),
	      res( alg.zero() )
	  {
		
	  }

	  

	  
	  auto operator()(const typename math::lie::group<domain>::algebra& v) const 
	    -> decltype( dexp(res) )
	  {
	    
	    v.each([&](natural i) {
		using math::euclid::operator+;
		using math::euclid::operator*;
		
		natural j = 0;
		alg.each(res, [&](real& r) {
		    auto delta = v(i) * alg.coord(j++, this->dirs(i));

		    if(i == 0 ) r = delta;
		    else r += delta;
		  });
		
	      });
	    
	    return dexp(res);
	  }

	};

	struct pull {
	  const math::vector<algebra>& dirs;

	  math::euclid::space< typename math::lie::group<range>::coalgebra > coalg;
	  
	  typedef typename traits< expontential >::pull dTexp_type;
	  dTexp_type dTexp;

	  mutable typename math::lie::group<domain>::coalgebra res;

	  pull(const ccfk& of,
	       const domain& at)
	    : dirs(of.dirs),
	      coalg( of.lie.coalg() ),
	      dTexp( dT(of.lie.exp())(of.help(at)))
	  {
	    res.resize( dirs.size() );
	  }
	  
	  
	  const typename math::lie::group<domain>::coalgebra& 
	  operator()(const typename math::lie::group<range>::coalgebra& f) const {
	    auto& pulled = dTexp(f);
	    
	    res.each([&](natural i) {
		res(i) = coalg.pair(this->dirs(i), pulled);
	      });
	    
	    return res;
	  }
	  
	};

	
	// T<range> diff(const T<domain>& dx) const {
	//   assert(dx.at().rows() == int(n));
	//   assert(dx.body().rows() == int(n));
	  
	//   typename Lie<G>::algebra at = lie.alg().zero();
	//   typename Lie<G>::algebra body = lie.alg().zero();
	 
	//   dx.at().each([&](natural i) { 
	//       // using math::euclid::operator+;
	//       using math::euclid::operator*;
	      
	//       const real xi = dx.at()(i);
	//       if( xi ) this->op(at, xi, this->dirs(i));
	      
	//       const real dxi = dx.body()(i);
	//       if( dxi ) this->op(body, dxi, this->dirs(i));
	//     });
	  
	//   return d(lie.exp())( math::body(std::move(at), std::move(body)) );
	// }

	
	// // TODO we compute exp twice FIXME
	// coT<domain> diffT(const pullT<ccfk>& fx) const {
	//   assert(fx.at().rows() == int(n));
	//   assert( math::euclid::dim(fx.body()) == lie.coalg().dim());
	  
	//   typename Lie<G>::algebra at = lie.alg().zero();
	  
	//   fx.at().each([&](natural i) {
	//       using math::euclid::operator+;
	//       using math::euclid::operator*;
	//       at = at + fx.at()(i) * this->dirs(i);
	//     });
	  
	//   // pull over coordinates:
	//   auto fat = math::pull( lie.exp(), at, fx.body() );
	  
	//   typename Lie<domain>::coalgebra body;
	//   body.resize(n);
	  
	//   body.parallel([&](natural i ) {
	//       body(i) = math::euclid::pair(fat.body(), this->dirs(i) );
	//     });
	  
	//   // done
	//   return math::cobody(fx.at(), std::move(body));
	// }

      };
  
    }
  }
}


#endif
