#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include <math/types.h>
#include <utility>
#include <core/parallel.h>
#include <math/eigen.h>

#include <meta/enable_if.h>

namespace math {
  
  template<class U, int M>
  struct vector : Eigen::Matrix<U, M, 1> {
    
    typedef Eigen::Matrix<U, M, 1> base;
    
    template<class ... Args>
    vector( Args&& ... args) 
      : base(std::forward<Args>(args)...) { 
    }

    vector(vector&& other) { 
      if( M == Eigen::Dynamic ) this->swap( std::move(other) ); 
      else *this = other;
    }

    vector(const vector& ) = default;
    vector& operator=(const vector&) = default;

    vector& operator=(vector&& other) {
      if(this == &other) return *this;
      if( M == Eigen::Dynamic ) this->swap( std::move(other) ); 
      else *this = other;
      return *this;
    }

    template<typename Other>
    vector& operator= (Other&& other)
    {
      base::operator=( std::forward<Other>(other));
      return *this;
    }
    
    
    template<class Action>
    void each( const Action& action ) const {
      for(natural i = 0, m = this->rows(); i < m; ++i) {
	action(i);
      }
    }

    template<class Action>
    void parallel( const Action& action ) const {
      
      // core::parallel(0, this->rows(), [&](int start, int end) { 
      const natural n = this->rows();
#pragma omp parallel
      {
	auto copy = action;
#pragma omp for
	for(natural i = 0; i < n; ++i) {
	  copy(i);
	}
      }
	// });
      
    }
    
    typedef typename base::ConstSegmentReturnType rchunk;
    typedef typename base::SegmentReturnType lchunk;
    
    // convenience
    rchunk full() const {
      return this->segment(0, this->size());
    }

    
    bool empty() const { return !this->rows(); }

    template<class F>
    static vector map(const F& f, natural m = M)  {
      vector res;

      assert( int(m) != -1 );

      res.resize(m);
      res.each( [&](natural i) {
	  res(i) = f(i);
	});

      return res;
    }

    
    static constexpr natural static_size() {
      static_assert( M != -1, "should not be called for dynamic vectors lol");
      return M;
    };
    
  };




  namespace euclid {

    // only for evenly-sized elements !
    template<class E, int M>
    struct traits< vector<E, M> > {
      typedef vector<E, M> vec;
      
      typedef typename space<E>::field field;
      typedef covector< typename space<E>::dual, M > dual;
      
      natural n;
      space<E> sub;
      
      traits(natural n = vec::static_size(), const space<E>& sub = space<E>())
	: n(n), sub(sub) { 
	assert( n );
      }

      traits(const vec& x) : n(x.rows()), sub(x(0)) {
	assert( n );
      }

      vec zero() const { 
	return vec::map([&](natural ) { return sub.zero(); }, n ); 
      }
      
      vec minus(const vec& x) const { 
	return vec::map( [&](natural i){ return sub.minus( x(i) ); }, n ); 
      }
      
      vec sum(const vec& x, const vec& y) const { 
	return vec::map( [&](natural i){ return sub.sum( x(i), y(i) ); }, n );
      } 
      
      vec scal(field lambda, const vec& x) const { 
	return vec::map( [&](natural i) { return sub.scal(lambda, x(i)); }, n );
      }
      
      space< dual > operator*() const {
	return { n, *sub };
      }
      
      natural dim() const { return n * sub.dim(); }
      
      field& coord(natural i, vec& x) const {
	return sub.coord( i % sub.dim() , x( i / sub.dim() ) ); 
      }

      const field& coord(natural i, const vec& x) const {
	return sub.coord( i % sub.dim(), x( i / sub.dim() )  ); 
      }


    };

  }

  namespace lie {

    template<class G, int M>
    struct traits< vector<G, M> > {

      typedef vector< typename group<G>::algebra, M > algebra;
      
      typedef vector<G, M> vec;
      
      natural n;
      group<G> sub;

      traits(natural n = vec::static_size(), 
	     const group<G>& sub = group<G>() )
	: n(n), sub(sub) { 
	assert( n );
      }

      traits(const vec& x) : n(x.rows()), sub(x(0)) {
	assert( n );
      }
      
      vec id() const { 
	return vec::map([&](natural ) { return sub.id(); }, n ); 
      }
      
      vec inv(const vec& x) const { 
	return vec::map( [&](natural i){ return sub.inv( x(i) ); }, n ); 
      }
      
      vec prod(const vec& x, const vec& y) const { 
	return vec::map( [&](natural i){ return sub.prod( x(i), y(i) ); }, n );
      } 
      

      struct adjoint : func::base< func::vector< typename group<G>::adjoint, M > > {
	
	adjoint(const vec& at) 
	  : adjoint::base( adjoint::base::data_type::map( [&](natural i){
		return lie::ad(at(i));
	      }, at.rows() ) ) {
	}

      };

      struct coadjoint : func::base< func::covector< typename group<G>::coadjoint, M > > {
	coadjoint(const vec& at) 
	  : coadjoint::base( coadjoint::base::data_type::map( [&](natural i){
		return lie::adT(at(i));
	      }, at.rows() ) ) {
	}
      };

      struct exponential : func::base< func::vector< typename group<G>::exponential, M> > {
	exponential(const group<vec>& g ) 
	  : exponential::base( exponential::base::data_type::map([&](natural ) {
		return g.impl.sub.exp();
	      }, g.impl.n)) {
	  
	}
      };

      struct logarithm : func::base< func::vector< typename group<G>::logarithm, M> > {
	logarithm(const group<vec>& g ) 
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
  bool nan(const Eigen::Matrix<U, M, 1>& vec) {
    
    for(natural i = 0, n = vec.rows(); i < n; ++i) {
      if( nan( vec(i) ) ) { return true; }
    }
    return false;
  }
  
  
}

// Eigen fix for matrices of vectors
// Eigen fixes
namespace Eigen {

  
  // template<int M> struct NumTraits< math::vector<math::real, M> >
  // {
  //   typedef math::real Real;
  //   typedef math::real NonInteger;
  //   typedef math::real Nested;
    
  //   enum {
  //     IsComplex = 0,
  //     IsInteger = 0,
  //     IsSigned,
  //     ReadCost = M,
  //     AddCost = M,
  //     MulCost = M
  //   };
    
  // };

}










namespace core {

  // template<class U, int M>
  // struct debugger< math::vector<U, M> > {
  //   typedef math::vector<U, M> vec;
  //   const vec& what;
    
  //   macro_self(debugger);

  //   macro_flag(size);
  //   macro_flag(content);
  //   macro_flag(nans);

  //   debugger(const vec& m) :
  //     what(m)      
  //   {
  //     size(true);
  //     content(false);
  //     nans(false);
  //   }

  //   std::ostream& operator()(std::ostream& o) const {
  //     if( size() ) {
  // 	o << "size: " << what.rows() << std::endl;
  //     }

  //     if( content() ) {
  // 	o << "content: " << std::endl
  // 	  << what.transpose() << std::endl;
  //     }
      
  //     if( nans() ) {
  // 	o << (math::nan(what) ? "nans detected !" : "no nans") << std::endl;
  //     }
      
  //     return o;
  //   }

  // };




}
  

namespace Eigen {

  namespace internal {
    template<class U , int M>
    struct traits< math::vector<U, M> > : 
      traits< typename math::vector<U, M>::base > {
    };
  }
}




#endif
