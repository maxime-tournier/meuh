#include "mvee.h"

#include <math/error.h>

#include <math/svd.h>
#include <math/pca.h>
#include <Eigen/LU>

#include <core/log.h>
#include <math/pi.h> 

namespace math {
  namespace algo {

    mvee::ellipsoid::ellipsoid(const mat& metric,
			       const vec& center)
      : metric(metric),
	center(center) {
      if( metric.rows() != metric.cols() ) throw error("need a square symmetric matrix");
      
      if( center.empty() ) this->center = vec::Zero(metric.rows());
      
      if( center.rows() != metric.rows() ) throw error("wrong center dimension");
    }

    bool mvee::ellipsoid::contains(const vec& x ) const {
      return distance_squared(x) <= 1;
    }

    real mvee::ellipsoid::distance(const vec& x ) const {
      return std::sqrt( distance_squared(x) );
    }

    real mvee::ellipsoid::distance_squared(const vec& x ) const {
      if( x.rows() != center.rows() ) throw error("wrong dimension");
      return ((x - center).transpose() * metric * (x - center))(0);
    }


    vec mvee::ellipsoid::project(const vec& x) const {
      if(!contains(x) ) {
	return center + (x - center) / distance(x);
      } else {
	return x;
      }
    }
    

    real mvee::ellipsoid::volume() const {
      return 4.0 / 3.0 * pi / std::sqrt( metric.determinant() );
    }

    //  value and index of largest element in v
    std::pair<real, int> max(const vec& v) {
      assert( v.rows() );
      std::pair<real, int> res = std::make_pair(v(0), 0);
    
      for(int i = 1, e = v.rows(); i < e; ++i) {
	if( v(i) > res.first)  {
	  res.first = v(i);
	  res.second = i;
	}
      
      }
      return res;
    }


    

    // adapted from Minimum Volume Enclosing Ellipsoid by Nima
    // Moshtagh
    // http://www.mathworks.com/matlabcentral/fileexchange/9542-minimum-volume-enclosing-ellipsoid
    mvee::ellipsoid mvee::operator()(const mat& P, const algo::stop& s , real fuzz) const {
      const math::real inf = 1e4;

      assert(!nan(P));
      
      // dimension reduction to avoid degenerate cases
      math::pca pca(P.transpose(), 1.0 / inf); // oh noes !1

      if(pca.zero() ) { 
      	return mvee::ellipsoid( inf * mat::Identity(P.rows(), P.rows()), 
      				vec::Zero(P.rows()) ); 
      }
      
      if(!pca.full()) {
	
      	const mvee::ellipsoid reduced = (*this)( pca.coords().leftCols( pca.rank()).transpose(), s );
	
      	return mvee::ellipsoid(  pca.space() * reduced.metric * pca.space().transpose()
      				 + inf * pca.orth() * pca.orth().transpose() ,
      				 pca.mean() + pca.space() * reduced.center );
      }


      // data points
      const natural d = P.rows(),  N = P.cols();
      mat Q;
      Q.resize(d+1, N);
      
      Q << 
	P + (fuzz * (mat::Random(d, N ) - mat::Constant(d, N, 1.0)) ), // on fuzze un peu pour eviter les cas degeneres
	vec::Ones(N).transpose();
    
      // initializations
      vec u = vec::Constant(N, 1.0 / N);
      
      // khachiyan algorithm
      algo::iter_eps(s, [&](real& eps) {
	  
	  const mat X = Q * u.asDiagonal() * Q.transpose();
	  const vec M = (Q.transpose() * X.inverse() * Q).diagonal();
	
	  real maximum; int j;

	  std::tie(maximum, j) = max(M);
	
	  const real step_size = (maximum - d - 1) / ((d+1) * (maximum - 1) );
	  vec new_u = (1 - step_size) * u;
	  new_u(j) += step_size;

	  eps = (new_u - u).norm();

	  u = new_u;
	 
	});
     
      
      // center of the ellipse
      const vec c = P * u;
      
      // A matrix for the ellipse
      const mat A = (1.0 / d) * (P * u.asDiagonal() * P.transpose() - c * c.transpose()).inverse();
      
      if( !A.symmetric(1e-5) ) { core::log()("warning", "non symmetric metric !"); }

      // TODO be a bit nicer than just fail
      // assert( A.transpose() == A && "degenerate result !" );
      const auto res = ellipsoid(A, c);

      P.each_col([&](natural j) {
	  
	  if( !res.contains(P.col(j))) { 
	    // const auto d = res.distance(P.col(j));
	    // if( d > 1.01 ) core::log()("oops !", d);
			
	  }
	});
      

      return res;

    }


  }

}
