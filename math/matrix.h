#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include <math/types.h>
#include <math/eigen.h>

#include <utility>
#include <core/parallel.h>

namespace math {
  
  template<class U, int M , int N >
  struct matrix : Eigen::Matrix<U, M, N> {
    
    typedef Eigen::Matrix<U, M, N> base;
    
    typedef U value_type;

    template<class ... Args>
    matrix( Args&& ... args) 
      : base(std::forward<Args>(args)...) { 
    }

    matrix(matrix&& other) : base() { 
      if( M == Eigen::Dynamic || N == Eigen::Dynamic ) this->swap( std::move(other) ); 
      else *this = other;
    }

    matrix(const matrix& ) = default;
    matrix& operator=(const matrix& ) = default;

    matrix& operator=(matrix&& other) {
      if( (M == Eigen::Dynamic) || (N == Eigen::Dynamic) ) this->swap( std::move(other) ); 
      else *this = other;
      return *this;
    }

    template<class Other>
    matrix& operator=(Other&& other) {
      base::operator=(std::forward<Other>(other));
      return *this;
    }
    
    template<class Action>
    void each_row( const Action& action ) const {
      for(natural i = 0, m = this->rows(); i < m; ++i) {
	action(i);
      }
    }
    
    template<class Action>
    void each_col( const Action& action ) const {
      for(natural j = 0, n = this->cols(); j < n; ++j) {
	action(j);
      }
    }
    
    template<class Action>
    void parallel_col(const Action& action) const {
      core::parallel(0, this->cols(), [&](int start, int end) {
	  for(int i = start; i < end; ++i) {
	    action(i);
	  }
	});
    }
    
    template<class Action>
    void each( const Action& action ) const {
      this->each_row([&]( natural i ) {
	  this->each_col([&]( natural j) {
	      action(i, j);
	    });
	});
    }
    
    bool empty() const { return !this->rows() || !this->cols(); }
    bool symmetric(U eps = 0) const { return (*this - this->transpose()).squaredNorm() <= eps; }

    
    using base::operator();
    
    template<class Vec>
    auto operator()(const Vec& v) const -> decltype( std::declval<base>() * v ) {
      return (*this) * v;
    }

  };
  


  template<class U, int M, int N>
  bool nan(const Eigen::Matrix<U, M, N>& mat) {
    
    for(int i = 0, n = mat.rows(); i < n; ++i) {
      for(int j = 0, m = mat.cols(); j < m; ++j) {
	if( nan( mat(i) ) ) { return true; }
      }
    }
    return false;
  }
  
}

// namespace core {

//   template<class U, int M, int N>
//   struct debugger< math::matrix<U, M, N> > {
//     typedef math::matrix<U, M, N> mat;
//     const mat& what;
    
//     macro_self(debugger);

//     macro_flag(size);
//     macro_flag(content);
//     macro_flag(nans);

//     debugger(const mat& m) :
//       what(m)      
//     {
//       size(true);
//       content(false);
//       nans(false);
//     }

//     std::ostream& operator()(std::ostream& o) const {
//       if( size() ) {
// 	o << "size: " << what.rows() << " x " << what.cols() << std::endl;
//       }

//       if( content() ) {
// 	o << "content: " << std::endl
// 	  << what << std::endl;
//       }
      
//       if( nans() ) {
// 	o << (math::nan(what) ? "nans detected !" : "no nans") << std::endl;
//       }
      
      
//       return o;
//     }

//   };




// }


namespace Eigen {
  namespace internal {

    template<class U , int M, int N >
    struct traits< math::matrix<U, M, N> > : 
      traits< typename math::matrix<U, M, N>::base > {
      
    };

  }

}



  
#endif
