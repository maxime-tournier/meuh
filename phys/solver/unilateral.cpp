#include "unilateral.h"

#include <math/algo/pgs.h>
#include <math/lcp.h>

#include <phys/solver/any.h>
#include <sparse/linear.h>

#include <phys/error.h>
#include <core/macro/here.h>
#include <math/algo/mosek.h>

#include <iostream>
#include <math/svd.h>

namespace phys {
  namespace solver {


    math::algo::mosek_qp* make_qp(const math::mat& compliance) {
      using namespace math;
      
      algo::mosek_qp* res = new algo::mosek_qp( math::svd(compliance).fix(1e-8), compliance);
      const natural n = compliance.cols();
      res->mosek.bounds(MSK_BK_LO,  vec::Zero(n), vec::Constant(n, +MSK_INFINITY) );
      return res;
    }

    unilateral::unilateral(const constraint::unilateral::pack& pack,
			   const solver::any& bilateral,
			   math::natural iter) 
      :  pack(pack),
	 data(pack.matrix, bilateral),
	 bilateral(bilateral),
	 pgs( pack.matrix.empty() ? 0 : 
	      // new math::algo::pgs( data.compliance(1e-14),
	      // 			   math::mat::Identity(data.keys.dim(), data.keys.dim())) ),
	      
	      make_qp( data.compliance() )),
	 iter(iter)
    {
      
    };

    // main computation
    math::vec lambda(const unilateral& solver, const dof::velocity& resp) {
      
      if(!solver.pgs) return math::vec();
      using namespace math;
      
      const vec n = solver.data.keys.dense(solver.pack.values) 
	- solver.data.dense * solver.data.dofs.dense(resp);
      assert( !math::nan(n) );
      
      // std::cout << "solving unilaterals" << std::endl;
      math::vec res = solver.pgs->solve(-0.5 * n, n);
      
      // math::vec res = solver.pgs->solve(c, algo::stop().it(solver.iter), solver.last);
      assert(! math::nan(res) );
      
      solver.last = res;
      return res;
    }


    unilateral::~unilateral() { }

    constraint::unilateral::vector unilateral::lambda(const dof::momentum& f) const {
      dof::velocity resp = bilateral(f);
      math::vec lambda = solver::lambda(*this, resp);
      
      if(lambda.empty() ) return constraint::unilateral::vector();
      return data.keys.sparse( lambda );
    }

    dof::velocity unilateral::operator()(const dof::momentum& f) const {
      const dof::velocity resp = bilateral(f);
      const math::vec lambda = solver::lambda(*this, resp);
      
      if( lambda.empty() ) return resp;
      
      using sparse::linear::operator+;
      return bilateral(f + data.dofs.sparse(data.dense.transpose() * lambda) );
    }

    dof::momentum unilateral::force(const dof::momentum& f) const {
      dof::velocity resp = bilateral(f);
      math::vec lambda = solver::lambda(*this, resp);

      if(lambda.empty() ) return dof::momentum();
      return data.dofs.sparse(data.dense.transpose() * lambda);
    }
    
    dof::momentum unilateral::force(const constraint::unilateral::vector& lambda) const {
      if(lambda.empty() ) return dof::momentum();

      return data.dofs.sparse( data.dense.transpose() * data.keys.dense(lambda));
    }
    
  }
}
