#include "reduced.h"

#include <phys/error.h>
#include <sparse/iter.h>

#include <math/svd.h>

namespace phys {
  namespace solver {


    struct helper {
      
      math::mat& res;
      const math::mat& space;
      
      void operator()(const dof* d, const math::mat& m, unsigned int o ) const {
	res.block(o, 0, sparse::linear::dim(d), space.cols()) =
	  m * space.block(o, 0, sparse::linear::dim(d), space.cols());
      }
      
      void operator()(const dof* , sparse::zero, unsigned int ) const {
	throw phys::error("no mass for dof !");
      }

      void operator()(const dof* , const math::mat& , sparse::zero ) const { };
       
    };


    
    reduced::inv_type* make_mi(const reduced& solver) { 
      if( solver.space.empty() ) return 0;
      
      const math::mat J = solver.data.dense.transpose();

      math::mat R;
      R.resize(J.rows(), J.cols());
      
      sparse::iter( core::range::all(solver.masses), 
		    solver.data.dofs.offsets(), helper{ R, J } );
      
      return new reduced::inv_type(J.transpose() * R);
    }


    math::vec response(const reduced& solver, const math::vec& f) {
      assert( solver.Mi );

      // J Mi JT
      return solver.data.dense.transpose() * solver.Mi->solve( solver.data.dense * f );
    }
    
    
    reduced::reduced(const dof::mass& masses,
		     const constraint::bilateral::matrix& space)
      : masses(masses),
	space(space),
	data(space),
	Mi( make_mi(*this)) {
      
    }
    
    dof::velocity reduced::operator()(const dof::momentum& f) const {
      if( !Mi ) return dof::velocity();
      return data.dofs.sparse( response(*this, data.dofs.dense(f)) );
    }


  }
}
