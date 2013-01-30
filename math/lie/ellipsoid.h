#ifndef MATH_LIE_ELLIPSOID_H
#define MATH_LIE_ELLIPSOID_H

#include <math/lie.h>
#include <math/euclid.h>
#include <math/algo/mvee.h>
#include <cmath>

#include <core/log.h>

namespace math {
  namespace lie {

    template<class G>
    struct ellipsoid {
      G center;
      mat metric;

      typedef typename Lie<G>::algebra algebra;
      typedef typename Euclid<algebra>::field field;

      field dist2(const G& g) const {
	assert( !metric.empty() ); 
	
	using lie::operator*;
	
	const Lie<G> lie(center);
	const Euclid<algebra> alg = lie.alg();
	
	const G diff = lie.inv(center) * g;
	const typename Euclid<algebra>::coordinates dir = alg.coords( lie.log()(diff) );
	
	return dir.dot( metric * dir );
      }

      field dist( const G& g ) const { 
	return std::sqrt( dist2(g) ); 
      }
      
      bool contains(const G& g) const {
	return dist(g) <= 1;
      }
      
      G proj( const G& g ) const {
	using lie::operator*;
	
	const Lie<G> lie(center);

	const G diff = lie.inv(center) * g;
	const algebra dir = lie.log()(diff);
	
	const field theta = dist(g);
	
	if(theta <=  1 ) return g;
	
	using euclid::operator/;
	return center * lie.exp()( dir / theta );
      }
      

    };
    

    template<class G>
    ellipsoid<G> fit_ellipsoid ( const math::vector<G>& data, const Lie<G>& lie = Lie<G>(),
				 const algo::stop& outer = algo::stop(), // tangent
				 const algo::stop& inner = algo::stop(), // mvee
				 real fuzz = 0)  {
      const Lie< math::vector<G> > vec_lie(data);
      
      G center = lie.id();
      G delta = lie.id();
      const algo::mvee mvee{};

      ellipsoid<G> res;

      const auto ret = algo::iter_eps( outer, [&](real& eps) {

	  center = center * delta;
	  // dirs
	  const G inv = lie.inv(center);
	  
	  const typename Lie< math::vector<G> >::algebra dirs = 
	  vec_lie.log()( data.unary([&](const G& g) -> G { return lie.prod(inv, g); } ) );

	  
	  // vertical samples
	  mat samples;
	  samples.resize( lie.alg().dim(), data.rows() );

	  dirs.each([&](natural i ) {
	      samples.col(i) = lie.alg().coords( dirs(i) );
	    });
      
	  // TODO change mvee behavior to avoid transpose
	  const algo::mvee::ellipsoid e = mvee(samples, inner, fuzz);
	  eps = e.center.norm() / e.center.rows(); 
	  
	  delta = lie.exp()(lie.alg().elem(e.center) );
      
	  res.center = center;
	  res.metric = e.metric;
	
	  // core::log()(e.center.norm(), e.volume() );
  
	});

      if( ret.iterations == outer.iterations ) { 
	core::log()("warning !", "no convergence !", ret.epsilon );
      } 

      return res;
    }


  }
}

#endif
