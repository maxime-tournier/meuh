#ifndef MATH_COVECTOR_H
#define MATH_COVECTOR_H

#include <math/types.h>
#include <math/eigen.h>
#include <utility>

// #include <core/parallel.h>
#include <core/unused.h>

namespace math {

  
  template<class U, int M >
  struct covector  : Eigen::Matrix<U, 1, M> {
    typedef Eigen::Matrix<U, 1, M> base;

    template<class ... Args>
    covector( Args&& ... args) 
      : base(std::forward<Args>(args)...) { 
    }
    
    covector(const covector& ) = default;
    covector& operator=(const covector& ) = default;
    
    covector(covector&& other) { 
      if( M == Eigen::Dynamic ) this->swap( std::move(other) ); 
      else *this = other;
    }

    covector(const covector&& other) { 
      if( M == Eigen::Dynamic ) this->swap( std::move(other) ); 
      else *this = other;
    }
    
    covector& operator=(covector&& other) {
      if(this == &other) return *this;
      if( M == Eigen::Dynamic ) this->swap( std::move(other) ); 
      else *this = other;
      return *this;
    }

    template<typename Other>
    covector& operator=(Other&& other)
    {
      base::operator=( std::forward<Other>(other));
      return *this;
    }
    

      
    template<class Action>
    void each( const Action& action ) const {
      for(natural i = 0, m = this->cols(); i < m; ++i) {
	action(i);
      }
    }

    template<class Action>
    void parallel( const Action& action ) const {

      // core::parallel(0, this->cols(), [&](int start, int end) { 
      const natural n = this->cols();
#pragma omp parallel for
      for(natural i = 0; i < n; ++i) {
	action(i);
      }
	// });

    }
    
    bool empty() const { return !this->cols(); }
    

    template<class F>
    static covector map(const F& f, natural m = M)  {
      covector res;
      res.resize(m);
      res.each( [&](natural i) {
	  res(i) = f(i);
	});

      return res;
    }
    
  };


  namespace euclid {

    template<class E, int M>
    struct traits< covector<E, M> > {
      typedef covector<E, M> covec;
      
      typedef typename space<E>::field field;
      typedef vector< typename space<E>::dual, M > dual;

      natural n;
      space<E> sub;
      
      traits(natural n = M, const space<E>& sub = space<E>() ) 
	: n(n), sub(sub) { 
	assert( n );
	assert( int(n) != size::dynamic);
      }
       
      traits(const covec& x) : n(x.cols()), sub( x(0) ) { 
	assert( n );
      }

      
      covec zero() const { 
	return covec::map([&](natural ) { return sub.zero(); }, n ); 
      }
      
      covec minus(const covec& x) const { 
	return covec::map( [&](natural i){ return sub.minus( x(i) ); }, n ); 
      }
      
      covec sum(const covec& x, const covec& y) const { 
	return covec::map( [&](natural i){ return sub.sum( x(i), y(i) ); }, n );
      } 
      
      covec scal(field lambda, const covec& x) const { 
	return covec::map( [&](natural i) { return sub.scal(lambda, x(i)); }, n );
      }
      
      space< dual > operator*() const {
	return {n, *sub};
      }

      natural dim() const { return n * sub.dim(); }

      field& coord(natural i, covec& x) const {
	return sub.coord( i % sub.dim(), x( i / sub.dim() ) ); 
      }

      const field& coord(natural i, const covec& x) const {
	return sub.coord( i % sub.dim(), x( i / sub.dim() )  ); 
      }
      
    };

  }
  
   namespace lie {
     
    template<class G, int M>
    struct traits< covector<G, M> > {

      typedef covector< typename group<G>::algebra, M > algebra;
      
      typedef covector<G, M> covec;
      
      natural n;
      group<G> sub;

      traits(natural n = M, const group<G>& sub = group<G>() )
	: n(n), sub(sub) { 
	assert( n );
      }

      traits(const covec& x) : n(x.rows()), sub(x(0)) {
	assert( n );
      }

      covec id() const { 
	return covec::map([&](natural ) { return sub.id(); }, n ); 
      }

      covec inv(const covec& x) const { 
	return covec::map( [&](natural i){ return sub.inv( x(i) ); }, n ); 
      }
      
      covec prod(const covec& x, const covec& y) const { 
	return covec::map( [&](natural i){ return sub.prod( x(i), y(i) ); }, n );
      } 
      

      struct adjoint : func::base< func::covector< typename group<G>::adjoint, M > > {
	
	adjoint(const covec& at) 
	  : adjoint::base( adjoint::base::data_type::map( [&](natural i){
		return lie::ad(at(i));
	      }, at.cols() ) ) {
	}

      };

      struct coadjoint : func::base< func::covector< typename group<G>::coadjoint, M > > {
	coadjoint(const covec& at) 
	  : coadjoint::base( coadjoint::base::data_type::map( [&](natural i){
		return lie::adT(at(i));
	      }, at.cols() ) ) {
	}
      };

      struct exponential : func::base< func::covector< typename group<G>::exponential, M> > {
	exponential(const group<covec>& g ) 
	  : exponential::base( exponential::base::data_type::map([&](natural ) {
		return g.impl.sub.exp();
	      }, g.impl.n)) {
	  
	}
      };

      struct logarithm : func::base< func::covector< typename group<G>::logarithm, M> > {
	logarithm(const group<covec>& g ) 
	  : logarithm::base( logarithm::base::data_type::map([&](natural ) {
		return g.impl.sub.log();
	      }, g.impl.n)) {
	  
	}
      };

      euclid::space< algebra > alg() const {
	return {n, sub.alg() };
      }
            
    };


  }



  template<class U, int M>
  bool nan(const Eigen::Matrix<U, 1, M>& covec) {
    
    for(natural i = 0, n = covec.rows(); i < n; ++i) {
      if( nan( covec(i) ) ) { return true; }
    }
    return false;
  }


}


namespace Eigen {
  namespace internal {
    template<class U , int M>
    struct traits< math::covector<U, M> > : 
      traits< typename math::covector<U, M>::base > {
      
    };

  }
}

  

#endif
