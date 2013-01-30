#ifndef PHYS_SOLVER_RESPONSE_H
#define PHYS_SOLVER_RESPONSE_H

#include <phys/solver/task.h>

#include <sparse/dense.h>

namespace phys {
  namespace solver {


    template<class Key>
    struct response {
      const typename Key::matrix matrix;

      const typename Key::matrix::index_type index;

      math::mat linear;
      math::mat dense_matrix;
      
      template<class Solver>
      response(const Solver& solver, const typename Key::matrix& matrix)
	: matrix(matrix),
	  index(matrix.index())
      {
	if( matrix.empty() ) return;
	
	linear.resize(matrix.cols().dim(), matrix.rows().dim());
	dense_matrix.resize(matrix.rows().dim(), matrix.cols().dim() );
	
	core::each( core::all(index), [&](typename Key::key c, core::unused ) {
	    
	    for(math::natural j = 0; j < sparse::dim(c); ++j) {

	      // TODO optimize alloc
	      dof::velocity v;
	      solver::task task(v);
	      task.momentum = this->row(c, j);
	      
	      solver.solve(task);

	      const math::natural col = this->matrix.rows()(c) + j;
	      linear.col( col ) = sparse::dense(this->matrix.cols(), v);
	      dense_matrix.row( col ) = sparse::dense(this->matrix.cols(), task.momentum).transpose();
	    }
	    
	  });
      }

      // TODO optimize allocs
      dof::momentum row(typename Key::key c, math::natural i) const {
	auto it = index.find(c);
	  
	dof::momentum res(it->second.size());
	
	core::each(it->second, [&](dof::key d) {
	    res(d) = matrix(c, d).row(i).transpose();
	  });

	return res;
      }

    };

  }
}



#endif
