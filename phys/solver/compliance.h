#ifndef PHYS_SOLVER_COMPLIANCE_H
#define PHYS_SOLVER_COMPLIANCE_H

#include <phys/solver/response.h>


namespace phys {
  namespace solver {
    

    template<class Key>
    struct compliance : response<Key> {
	
      math::mat dense;
	
      template<class Solver>
      compliance(const Solver& solver, 
		 const typename Key::matrix& matrix,
		 math::real epsilon = 0)
	: response<Key>(solver, matrix) {
	  
	if( matrix.empty() ) return;
	
	dense.resize( matrix.rows().dim(), matrix.rows().dim() );
	
	// TODO: parallel ?
	dense.each_col([&](math::natural j) {
	    using sparse::operator*;
	    dense.col(j) = sparse::dense( matrix.rows(), matrix * sparse::dense(matrix.cols(), this->linear.col(j)));
	  });
	
	dense.diagonal() += math::vec::Constant(matrix.rows().dim(), epsilon );
      }

      template<class Solver>
      compliance(const Solver& solver, 
		 const typename Key::matrix& matrix,
		 const typename Key::vector& damping)
	: response<Key>(solver, matrix) {
	
	if( matrix.empty() ) return;
	
	dense.resize( matrix.rows().dim(), matrix.rows().dim() );
	
	// TODO: parallel ?
	dense.each_col([&](math::natural j) {
	    using sparse::operator*;
	    dense.col(j) = sparse::dense( matrix.rows(), matrix * sparse::dense(matrix.cols(), this->linear.col(j)));
	  });
	
	dense.diagonal() += sparse::dense(matrix.rows(), damping);
      }
	
      compliance(const compliance& ) = default;
      compliance(compliance&& ) = default;
      
    };


  

  }
}


#endif
