#include "least_squares.h"

#include <math/algo/conjugate_gradient.h> 

#include <Eigen/SVD>
#include <math/space/matrix.h>

namespace math {
  namespace algo {

    namespace least_squares {
    
      
      mat normal(const mat& A) {
	return A.transpose() * A;
      }

      
      // solve
      solve::solve(const mat& A) : original(A), normal( least_squares::normal(A) ) { }
      solve::solve(const mat& A, const mat& normal) : original(A), normal( normal ) { }
      
      vec solve::operator()(const vec& b, const stop& s  ) const {
	return operator()(b, vec::Zero( original.cols() ), s);
      }
      
      vec solve::operator()( const vec& b, const vec& init, const stop& s ) const {
	vec res = normal.svd().solve(original.transpose() * b);
	
	return res;
	return cg_solve( normal ) ( original.transpose() * b, init, s );
      }
      
       // damped
      damped::damped(const mat& A, real epsilon) 
	: solve(A, least_squares::normal(A) + vec::Constant(normal.rows(), epsilon ).asDiagonal() ),
	  epsilon(epsilon)  { 

      }
      

    }
  
  }

}
