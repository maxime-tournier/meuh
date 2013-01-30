#include "gs.h"

namespace math {
  namespace algo {


    namespace proj {
      
      real none::operator()(const vec& data, natural i ) const { return data(i); }
  
      real pos::operator()(const vec& data, natural i) const { return std::max(data(i), 0.0); }
      real neg::operator()(const vec& data, natural i) const { return std::min(data(i), 0.0); }
      
      bounds::bounds(const vec& low, const vec& up) 
	: low(low), up(up) { 
	assert(low.rows() == up.rows());
      }
      
      real bounds::operator()(const vec& data, natural i) const {
	return std::max(std::min(data(i), up(i)), low(i));
      }
      
    }

    static vec diag(const mat& original, const real eps = 0) {
      return original.diagonal() + vec::Constant(original.rows(), eps);
    }

    static mat iter(const math::mat& original
	     ) {
      mat tmp = original;
      tmp.diagonal().setConstant( 0 );
      
      return tmp;
    }

    gs::gs(const mat& M, math::real eps ) 
      : diag(algo::diag(M, eps)),
	iter(algo::iter(M) ) {
      assert( ! (diag.array() == 0).any() );
    }
    
    real gs::step(const vec& x, const vec& q, natural i) const {
      return (q(i) - iter.row(i).dot(x)) / diag(i);
    }

    void gs::check(const vec& x) const {
      if( x.rows() != diag.rows() ) throw error("bad m'kay ?");
    }
    

  }
}
