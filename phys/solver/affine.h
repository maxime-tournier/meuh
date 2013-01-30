#ifndef PHYS_SOLVER_AFFINE_H
#define PHYS_SOLVER_AFFINE_H

#include <math/matrix.h>
#include <phys/dof.h>
#include <sparse/linear/dense.h>
#include <sparse/linear/numbering.h>

#include <core/parallel.h>


namespace phys {
  namespace solver {

    // only dense and sparse stuff
    template<class Key>
    struct chunk {
      const typename Key::matrix& matrix;

      sparse::linear::numbering<Key> keys;
      sparse::linear::numbering<phys::dof> dofs;

      math::mat dense;
      math::vec scaling;

      chunk(const typename Key::matrix& matrix) 
	: matrix(matrix) {
	if( matrix.empty() ) return;
	
	sparse::linear::number(keys, dofs, matrix);
	dense = sparse::linear::dense( keys, dofs, matrix);
	
	scaling.setOnes(keys.dim());
	// scaling = (dense.rowwise().norm().array() + 1).inverse();
	// dense = scaling.asDiagonal() * dense;
	
      }

      math::vec rhs(const typename Key::vector& b) const {
	assert(!matrix.empty());
	return scaling.cwiseProduct( sparse::linear::dense(keys, b) );
      }

    };
    
    
    // affine response precomputation for given impulses
    // this can be used with any bilateral solver
    template<class Key>
    struct affine : chunk<Key> {
      
      math::vec origin;
      math::mat linear;	// linear response

      template<class Solver>
      affine(const typename Key::matrix& matrix,
	     const Solver& solver)
	: chunk<Key>(matrix) {
	
	if( matrix.empty() ) return;
	
	const math::natural m = this->keys.dim();
	const math::natural n = this->dofs.dim();
	
	linear.resize(n, m);
	
	origin = sparse::linear::dense(this->dofs, solver( dof::momentum() ) );
	
	// split response computation
	auto task = [&](math::natural start, math::natural end) {
	    
	    // this is to avoid (re) allocations. thread local.
	    dof::momentum j_i = sparse::linear::vector(this->dofs, this->origin);
	    math::vec response = this->origin;
	    
	    for(math::natural i = start; i < end; ++i) {
	      sparse::linear::update(j_i, this->dofs, this->dense.row(i).transpose() );
	      sparse::linear::update(response, this->dofs, solver( j_i ) );
	      response -= this->origin;
	      
	      this->linear.col(i).noalias() = response;
	    }
	    
	};
	
	// core::parallel(0, m, task);
	task(0, m);

      }
      
      
      math::mat compliance(math::real damping = 0) const {
	if(origin.empty()) return math::mat();
	
	math::mat res = this->dense * linear;
	assert(res.rows() == res.cols());
	
	// res.diagonal().array() += damping;
	// return res;

	// project this on symmetric matrices
	// math::mat tmp = 0.5 * (res + res.transpose());
	
	// levmar damping
	res.diagonal() = (1 + damping) * res.diagonal();

	// // tikhonov damping
	// tmp.diagonal().array() += damping;
	

	
	return std::move(res);
      }
      
      
      // full response
      math::vec response(const math::vec& lambda) const {
	if(origin.empty()) return math::vec();
	return origin + linear * lambda;
      }

      
    };

  }
}


#endif
