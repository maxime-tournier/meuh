#include "pca.h"
#include <math/svd.h>

#include <core/log.h>
#include <math/error.h>

#include <Eigen/Cholesky>


namespace math {

  pca::pca() : rank_(0){ }

  pca::pca(mat samples, real eps) { 
    compute(samples, eps);
  }


  static void print_vec(const vec& eta) {
    
    mat zob = mat::Zero(eta.rows(), 2);

    eta.each([&](natural i) {
	zob.col(0)(i) = i;
	zob.col(1)(i) = eta(i);
      });
    
    core::log()("Cumulative variance:");
    core::log()(zob);
    
  }


  pca& pca::compute(mat samples, real eps, mat metric) {
    
    mean_ = samples.colwise().mean().transpose();
    samples.rowwise() -= mean().transpose();
    
    Eigen::LLT< mat > llt;

    if( !metric.empty() ) {
      assert( metric.rows() == samples.cols() );
      assert( metric.cols() == metric.rows() );
      
      llt.compute( metric );
       
      samples = samples * llt.matrixL();
    }
    
    math::svd svd;
    svd.compute(samples, eps, Eigen::ComputeThinV );
  
    if( !metric.empty() ) {
      basis_ = llt.matrixU().solve( svd.v() );
    } else {
      basis_ = svd.v();
    }
    
    // surprisingly, this one works for both cases (coords = samples *
    // U^{-T}) where U is the eigen basis
    coords_ = samples * svd.v();

    dev_ = svd.singular().cwiseAbs();
    
    rank_ = svd.rank();

    // print_vec( cumul() );

    return *this;
  }




  natural pca::dim() const { return dev().rows(); }
  
  mat pca::space() const throw(exception::zero) {
    if( zero()  ) throw exception::zero();
    return basis().leftCols(rank());
  }
  
  mat pca::orth() const throw(exception::full) {
    if( full() ) throw exception::full();
    return basis().rightCols( natural(mean().rows()) - rank());
  }


  vec pca::cumul() const {
    return cumul( dev() );
  }


  vec pca::cumul(const vec& eig) {
    vec res = vec::Zero(eig.rows());
    
    real sum = 0;

    eig.each([&](natural i ) {
	sum += eig(i) * eig(i);
	res(i) = sum;
      });
    assert( sum > 0 );
    
    res /= sum;

    return res;
  }


  natural pca::reduced(const vec& eig, real percent) {
    if( percent < 0 || percent > 1 ) throw error("variance must be in [0, 1]");

    vec c = cumul(eig);
    
    const natural n = eig.rows();
    for(natural i = 0; i < n; ++i) {
      if( c(i) > percent ) return i;
    }
    assert(false);
    
    return n;
  }

  natural pca::reduced(real percent) const {
    return reduced(dev(), percent);
  }

  natural pca::rank() const { return rank_; }

  const mat& pca::basis() const { return basis_; }
  const vec& pca::mean() const { return mean_; }
  const vec& pca::dev() const { return dev_; }
  const mat& pca::coords() const { return coords_; }
  
  bool pca::zero() const { return rank() == 0; }
  bool pca::full() const { 
    
    return rank() == natural( mean().rows() );
  }
}
