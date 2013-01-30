#include "svd.h"

#include <math/error.h>
#include <Eigen/Jacobi>

#include <Eigen/Geometry>

namespace math {

  namespace rank {

    // naive rank clamping
    natural naive(const vec& eig, real eps) {
      assert( eps >= 0 );
      
      natural ker = 0;
      for(natural i = 0, n = eig.rows();  i < n; ++i) {
	if( std::abs( eig(i) ) <= eps ) {
	  ++ker;
	}
      }
      // full rank
      return eig.rows() - ker;
    }

    // http://www.mathworks.com/access/helpdesk/help/techdoc/ref/rank.html
    natural matlab(const vec& eig, const mat& M, real eps) {
      const real tol = std::max( M.rows(), M.cols() ) * eps * eig.maxCoeff();
      // TODO cheapest way ?
      return (eig.array() > vec::Constant( eig.rows(),  tol ).array() ).count();
    }
      
  }
  

  svd::svd() : rank_(0) { }
  svd::svd(const mat& m, real eps ) { compute(m, eps); }


  bool svd::psd() const {
    return (eigen().array() >= 0).all();
  }


  bool svd::pd(real eps) const {
    return (eigen().array() >= eps).all();
  }


  svd& svd::compute(const mat& m, real eps, unsigned int options )
  { 
    assert( !m.empty() );
    
    const Eigen::JacobiSVD<mat> data( m, options);
      
    if(options & Eigen::ComputeThinU) { 
      u_ = data.matrixU();
      assert( u().rows() == m.rows() );
    }
    
    if(options & Eigen::ComputeThinV) {
      v_ = data.matrixV();
      assert( v().rows() == m.cols() );
    }

    singular_ = data.singularValues();
    assert( singular().rows() == std::min(m.rows(), m.cols()) );

    rank_ = // rank::matlab(singular(), m, eps);
      rank::naive(singular(), eps);
    
    return *this;
  }
    
  const mat& svd::u() const {
    assert( !u_.empty() );
    return u_;
  }

  const mat& svd::v() const {
    assert( !v_.empty() );
    return v_;
  }
    
  const vec& svd::singular() const { return singular_; }
  
  // only for symmetric !
  vec svd::eigen() const { 
    assert(u().cols() == v().cols());
    vec signs(singular().rows());

    signs.each([&](natural i ) {
  	signs(i) = u().col(i).dot(v().col(i));
	// assert( std::abs(std::abs(signs(i)) - 1) < 1e-3 );
      });
    
    // macro_debug(signs).content(true) << std::endl;

    return singular().cwiseProduct( signs );
  }

  mat svd::inverse() const throw( exception::zero ){

    if( zero() ) throw exception::zero();
            
    return
      v().leftCols( rank() ) * 
      singular().head( rank() ).cwiseInverse().asDiagonal() *
      u().leftCols( rank() ).transpose();
  }

 
  mat svd::fix(math::real eps) const {
    if( zero() ) return mat::Zero( u().rows(), v().rows() );
    
    natural k = rank::naive(singular(), eps);
    
    return
      u().leftCols(k) * singular().head(k).asDiagonal() * v().leftCols(k).transpose();
  }

  vec svd::solve( const vec& rhs ) const throw(exception::zero){
    if( zero() ) throw exception::zero();
    if( rhs.rows() != u().rows()) throw error("bad rhs");
    
    return
      v().leftCols(rank()) * 
      singular().head(rank() ).cwiseInverse()
      .cwiseProduct( u().leftCols(rank()).transpose() * rhs );
  }
  
  
  vec tikhonov_singular_inverse(const vec& eig, real eps ) {
    return eig.array() / 
      ( eig.array().square() + vec::Constant( eig.rows(), eps ).array().square() );
  }
    
  mat svd::tikhonov( real eps ) const {
    return 
      v() * tikhonov_singular_inverse(singular(), eps).asDiagonal() * u().transpose();
  }
    
  vec svd::tikhonov_solve( const vec& rhs, real eps ) const {
    return 
      v() * ( tikhonov_singular_inverse(singular(), eps).array() * (u().transpose() * rhs).array() ).matrix();
  }

    
  bool svd::zero() const {
    return rank() == 0;
  }


  bool svd::full() const {
    return +rank() == u().cols();
  }


  natural svd::rank() const {
    return rank_;
  }
    
  mat svd::im( ) const throw( exception::zero ) {
    if( zero() ) throw exception::zero();
      
    return u().leftCols( rank() );
  }
    
  mat svd::ker(  ) const throw( exception::full ){
    if( full() ) throw exception::full();
      
    return v().rightCols(v().cols() - rank() );
  }

  mat svd::ker_orth( ) const throw( exception::zero ) {
    if( zero() ) throw exception::zero();
      
    return v().leftCols( rank() );
  }


}


