#ifndef MATH_LIE_PGA_H
#define MATH_LIE_PGA_H

#include <math/lie.h>
#include <math/func/coord.h>
#include <math/func/lie.h>
#include <math/func/compose.h>
#include <math/func/lie/translation.h>
#include <math/iter.h>
// #include <math/func/lie/pga.h>
#include <math/func/lie/ccfk.h>
#include <math/func/lie/ccsk.h>
#include <math/lie/geometric_mean.h>
#include <math/lie/samples.h>


#include <math/pca.h>		// TODO move pca to euclid ?



namespace math {
  namespace lie {

    template<class G>
    struct pga {
      typedef G data_type;
      typedef typename group<G>::algebra algebra;
      typedef typename euclid::space<algebra>::field field;
      
      G mean;
      vec dev;
      vector< algebra > basis;
      
      natural dim() const { return dev.size(); }
      
      typedef std::function< mat (const G& ) > metric_type;
      
      pga(const vector<G>& data,
	  const math::iter& iter,
	  const group<G>& g = group<G>(),
	  real target = 1.0,
	  metric_type metric = 0) 
      {
	auto mean_algo = lie::mean(g);
	mean = mean_algo(data, iter);
	
	// tangent linearization around mean
	const vector<algebra> tangent = mean_algo.tangent(mean, data);
	
	auto samples = lie::samples<G>(g);
	
	// stick everything in a matrix
	matrix<field> raw = samples.coords( tangent );
	
	math::pca pca;
	
	if(! metric ) {
	  pca.compute( raw );
	} else {
	  mat M = metric( mean );

	  assert( M.rows() == raw.cols() );
	  assert( M.cols() == raw.cols() );

	  pca.compute( raw, math::epsilon, M );
	}
	
	basis = samples.elems( pca.basis().transpose() );
	dev = pca.dev();
	
	const natural res = pca.reduced( target );

	// std::cout << "pga reduction: " << res << " / " << pca.dim() << std::endl;

	// return res;
      }
     



      // math::natural exact(const vector<G>& data,
      // 			  const algo::stop& stop = algo::stop(),
      // 			  const Lie<G>& lie = Lie<G>(),
      // 			  math::real target = 1.0) {
      // 	mean_ = lie::mean(lie)(data, stop);
	
      // 	vector<G> rebuild = vector<G>::Constant(data.rows(), mean());
      // 	geodesics_.resize(lie.alg().dim());
      // 	eigen_.resize(lie.alg().dim());
	
      // 	const auto exp = lie.exp();

      // 	for(natural i = 0; i < lie.alg().dim(); ++i) {
      // 	  // tangent linearization around mean
      // 	  const vector<algebra> tangent = lie::log( lie::prod( lie::inv(rebuild), data) );
	  
      // 	  // stick everything in a matrix
      // 	  matrix<field> raw = samples<G>(lie).coords( tangent );

      // 	  // pca (derp)
      // 	  const math::pca pca( raw );

      // 	  // get first eigenvector
      // 	  geodesics_(i) = lie.alg().elem(pca.basis().col(0));
      // 	  eigen_(i) = pca.dev()(0);
	  
      // 	  // add reconstruction
      // 	  rebuild.each([&](natural j) {
      // 	      real coord = pca.coords()(j, 0);
      // 	      rebuild(j) = lie.prod(rebuild(j), exp( lie.alg().scal(coord, geodesics_(i))));
      // 	    });
      // 	}
	
      // 	return pca::reduced(eigen(), target);
      // }
      
      
      
      // pga( const vector<G>& data, const algo::stop& stop = algo::stop(), 
      // 	   const Lie<G>& lie = Lie<G>() ) {
      // 	compute(data, stop, lie);	
      // }      
      
      // pga() { }
      
      // vector<field> project(const G& g, natural nn = 0) const {
      // 	const natural n = nn ? nn : dim(); 
      // 	const algebra dir = log( inv(mean()) * g);
	
      // 	vector<field> res = vector<field>::Zero(n);
	
      // 	res.each([&](natural i) {
      // 	    res(i) = lie::of(this->mean()).alg().dot(this->geodesics()(i), dir);
      // 	  });
	
      // 	return res;
      // }
      

      func::compose< func::lie::left<G>,
		     func::lie::ccfk<G> > ccfk(natural n) const { 
	using func::operator<<;
	return func::lie::L(mean) << func::lie::ccfk<G>(basis, n, lie::of(mean));
      }

      func::compose< func::lie::left<G>,
		     func::lie::ccsk<G> > ccsk(natural n) const { 
	using func::operator<<;
	return func::lie::L(mean) << func::lie::ccsk<G>(basis, n, lie::of(mean));
      }
      
      // func::lie::pga<G, func::lie::ccfk> ccfk(natural n = 0) const { return {mean, basis, n, lie::of(mean)}; }
      // func::lie::pga<G, func::lie::ccsk> ccsk(natural n = 0) const { return {mean, basis, n, lie::of(mean)}; }
    
    };
  }

}

#endif
