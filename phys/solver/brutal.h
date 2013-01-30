#ifndef PHYS_SOLVER_BRUTAL_H
#define PHYS_SOLVER_BRUTAL_H

#include <phys/constraint/bilateral.h>

#include <phys/solver/affine.h>

#include <sparse/linear.h>

#include <phys/error.h>
#include <core/macro/here.h>

#include <Eigen/LU>
#include <Eigen/Cholesky>

#include <math/svd.h>
#include <sparse/linear/nan.h>

#include <phys/solver/result.h>

namespace phys {
  namespace solver {

    template<class Solver>
    class brutal {
    public:
      const affine<phys::constraint::bilateral> cache;
      const phys::constraint::bilateral::values& values;
      
      const Solver& solver;
      
    private:
      // typedef math::svd inv_type;
      typedef Eigen::LDLT< Eigen::MatrixXd > inv_type;
      const std::unique_ptr<inv_type> inv;
    public:
      
      brutal(const phys::constraint::bilateral::pack& pack,
	     const Solver& solver,
	     math::real eps = 0)
	: cache(pack.matrix, solver),
	  values(pack.values),
	  solver(solver),
	  inv( pack.matrix.empty() ? 0 : new inv_type(cache.compliance(eps) ) )
      {
	
      }
      
      dof::velocity operator()(const dof::momentum& f,
				 const result& res = result()) const {
	assert( ! sparse::linear::nan(f) );
	const dof::velocity resp = solver( f );
	
	if( !inv ) return std::move(resp);
	
	math::vec rhs = sparse::linear::dense(cache.keys, values);
	rhs.noalias() -= cache.dense * sparse::linear::dense(cache.dofs, resp);
	assert( !math::nan(rhs) );
	
	const math::vec lambda = inv->solve(rhs);
	
	assert( !math::nan(lambda) );

	if( res.lambda ) {
	  res.update_lambda( cache.keys.sparse(lambda) );
	}
	
	using sparse::linear::operator+;
	return solver( f + cache.dofs.sparse( cache.dense.transpose() * lambda ), res);
      };
    

    };

    
  }
}

#endif
